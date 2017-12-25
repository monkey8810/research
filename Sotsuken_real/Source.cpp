// リンカーの入力を設定
// デバッグとリリースで入力するファイルが異なるのでこのようにしています。
#ifdef _DEBUG
#define CV_EXT "d.lib"
#else
#define CV_EXT ".lib"
#endif
#pragma comment(lib, "opencv_world330" CV_EXT)

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string.h>
#include<fstream>
#include<string>
#include<sstream>
#include <vector>
#include <cstdlib>

using namespace cv;
using namespace std;

#define MAX_IMAGESIZE 16 // 読み込み画像数

//パス C:\\2017data\\CheckCheck\\CheckCheck.csv

//2次元配列の要素数カウントメソッド
int tokutyouten(int **data, int a)
{
	for (int i = 0; i < a; i++)
	{
		if (data[i][0] == 0)
		{
			return i;
		}
	}
	return a;
}

// 細線化メソッド
void thinningIte(cv::Mat& img, int pattern)
{
	cv::Mat del_marker = cv::Mat::ones(img.size(), CV_8UC1);
	int x, y;
	for (y = 1; y < img.rows - 1; ++y)
	{
		for (x = 1; x < img.cols - 1; ++x)
		{
			int v9, v2, v3;
			int v8, v1, v4;
			int v7, v6, v5;

			v1 = img.data[y   * img.step + x * img.elemSize()];
			v2 = img.data[(y - 1) * img.step + x * img.elemSize()];
			v3 = img.data[(y - 1) * img.step + (x + 1) * img.elemSize()];
			v4 = img.data[y   * img.step + (x + 1) * img.elemSize()];
			v5 = img.data[(y + 1) * img.step + (x + 1) * img.elemSize()];
			v6 = img.data[(y + 1) * img.step + x * img.elemSize()];
			v7 = img.data[(y + 1) * img.step + (x - 1) * img.elemSize()];
			v8 = img.data[y   * img.step + (x - 1) * img.elemSize()];
			v9 = img.data[(y - 1) * img.step + (x - 1) * img.elemSize()];

			int S = (v2 == 0 && v3 == 1) + (v3 == 0 && v4 == 1) +
				(v4 == 0 && v5 == 1) + (v5 == 0 && v6 == 1) +
				(v6 == 0 && v7 == 1) + (v7 == 0 && v8 == 1) +
				(v8 == 0 && v9 == 1) + (v9 == 0 && v2 == 1);

			int N = v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9;

			int m1 = 0, m2 = 0;

			if (pattern == 0) m1 = (v2 * v4 * v6);
			if (pattern == 1) m1 = (v2 * v4 * v8);

			if (pattern == 0) m2 = (v4 * v6 * v8);
			if (pattern == 1) m2 = (v2 * v6 * v8);

			if (S == 1 && (N >= 2 && N <= 6) && m1 == 0 && m2 == 0)
				del_marker.data[y * del_marker.step + x * del_marker.elemSize()] = 0;
		}
	}
	img &= del_marker;
}

void thinning(const cv::Mat& src, cv::Mat& dst) {
	dst = src.clone();
	dst /= 255;         // 0は0 , 1以上は1に変換される

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;

	do
	{
		thinningIte(dst, 0);
		thinningIte(dst, 1);
		absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (countNonZero(diff) > 0);

	dst *= 255;
}

//端点検出メソッド
int endpoint(cv::Mat& src, int **edge)
{
	int count = 0;
	for (int y = 1; y < src.rows - 1; y++)
	{
		for (int x = 1; x < src.cols - 1; x++)
		{
			int v4, v3, v2;
			int v5, v0, v1;
			int v6, v7, v8;
			//X座標がx, Y座標がyに位置するピクセルの値を取得
			v0 = src.at<unsigned char>(y, x);
			//std::cout << "v0:" << v0 << std::endl;
			v1 = src.at<unsigned char>(y, x + 1);
			//std::cout << "v1:" << v1 << std::endl;
			v2 = src.at<unsigned char>(y - 1, x + 1);
			//std::cout << "v2:" << v2 << std::endl;
			v3 = src.at<unsigned char>(y - 1, x);
			//std::cout << "v3:" << v3 << std::endl;
			v4 = src.at<unsigned char>(y - 1, x - 1);
			//std::cout << "v4:" << v4 << std::endl;
			v5 = src.at<unsigned char>(y, x - 1);
			//std::cout << "v5:" << v5 << std::endl;
			v6 = src.at<unsigned char>(y + 1, x - 1);
			//std::cout << "v6:" << v6 << std::endl;
			v7 = src.at<unsigned char>(y + 1, x);
			//std::cout << "v7:" << v7 << std::endl;
			v8 = src.at<unsigned char>(y + 1, x + 1);
			//std::cout << "v8:" << v8 << std::endl;

			int sum = abs(v1 + v2 + v3 + v4 + v5 + v6 + v7 + v8);
			//std::cout << sum << std::endl;
			if (sum == 1785 && v0 == 0)
			{
				edge[count][0] = x;	//x座標を代入
				edge[count][1] = y;	//y座標を代入
				count++;
			}
		}
	}
	return count;
}

int main(void)
{
	// 連番読み込み(オフライン画像)
	int src_size = 0;
	Mat search_img[MAX_IMAGESIZE];

	for (int i = 0; i < MAX_IMAGESIZE; i++)
	{
		search_img[i] = imread("C:\\img_latest\\offline\\" + to_string(i + 1) + ".bmp", 1);
		if (!search_img[i].data) break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		src_size++;
	}
	/*for (int i = 0; i < src_size; i++) {
		imshow("search_" + to_string(i + 1), search_img[i]); //入力された画像を表示するウィンドウを自動で生成
	}*/

	//オフライン読み込み
	//cv::Mat src_img_off = cv::imread("C:\\2017data\\CheckCheck\\kakiwari(01).bmp", 1);
	//cv::Mat src_img_off2;	//細線化用
	//if (src_img_off.empty()) return -1;

	//リサイズ(Lanczos法の補間)
	//resize(src_img_off, src_img_off, cv::Size(), 720.0 / src_img_off.cols, 1280.0 / src_img_off.rows, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5);
	//std::cout << "width_off: " << src_img_off.cols << std::endl;	//1080
	//std::cout << "height_off: " << src_img_off.rows << std::endl;	//1920

	//画像表示
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	//cv::imshow("Image1", src_img_off);
	//cv::imshow("Image2", src_img_off2);

	//csvファイル読み込み(連番)
	int i, j;

	for (int a = 0; a < MAX_IMAGESIZE; a++)
	{
		int divide_average1[30][4] = { 0 };
		int divide_average2[30][4] = { 0 };
		int divide_frame1[30][4] = { 0 };
		int divide_frame2[30][4] = { 0 };
		int divide_compare1[30][4] = { 0 };
		int divide_compare2[30][4] = { 0 };
		int online[30][4] = { 0 };	//x,y,フレーム数,方向角
		int *kawari[30];	//受け渡し用
		int edge[100][2] = { 0 };	//端点保存用配列
		int *p_edge[100];	//受け渡し用

		ifstream file("C:\\img_latest\\csv\\" + to_string(a + 1) + ".csv");
		string str;
		string token;

		//読めなかったとき
		if (!file)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "csvファイルの中身" << std::endl;
		i = 0;
		//csvファイルを1行ずつ読み込む
		while (getline(file, str))
		{
			j = 0;
			istringstream stream(str);
			//1行のうち、文字列とコンマを分割する
			while (getline(stream, token, ','))
			{
				//すべて文字列として読み込まれるため
				//数値は変換が必要
				online[i][j] = stoi(token);
				cout << online[i][j] << ",";
				//サイズ合わせ
				if (j == 0 || j == 1) {
					online[i][j] = online[i][j];
				}
				j++;
			}
			cout << endl;
			i++;
		}

		//確認
		cout << "csvファイルを移した配列の中身" << std::endl;
		for (i = 0; i < 30; i++)
		{
			for (j = 0; j < 4; j++)
			{
				std::cout << online[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = online[i];
		}

		//特徴点の数確認
		cout << "特徴点の数" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(online) / sizeof(online[0])) << std::endl;

		cout << "特徴点のみ表示" << std::endl;
		for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				std::cout << online[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//グレー画像化
		cv::cvtColor(search_img[a], search_img[a], CV_RGB2GRAY);

		//2値化
		cv::threshold(search_img[a], search_img[a], 1, 255, CV_THRESH_BINARY);

		//オフライン画像にない画素を削除
		for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
		{
			if (online[i][1] == 0 && online[i][0] == 0)
			{
				break;
			}
			int intensity = search_img[a].at<unsigned char>(online[i][1], online[i][0]);
			if (intensity == 255)
			{
				for (int h = i; h < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); h++)
				{
					online[h][0] = online[h + 1][0];
					online[h][1] = online[h + 1][1];
					online[h][2] = online[h + 1][2];
					online[h][3] = online[h + 1][3];
				}
				i--;
			}
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = online[i];
		}

		// 白黒反転(細線化用)
		bitwise_not(search_img[a], search_img[a]);

		// 細線化
		thinning(search_img[a], search_img[a]);

		// 白黒反転(戻し)
		bitwise_not(search_img[a], search_img[a]);

		//2次元配列の受け渡し用入れ替え
		for (int i = 0; i < 100; i++) p_edge[i] = edge[i];

		//端点検出
		std::cout << "端点の個数:" << endpoint(search_img[a], p_edge) << std::endl;

		// 確認
		cout << "端点の座標" << std::endl;
		for (int i = 0; i < tokutyouten(p_edge, sizeof(edge) / sizeof(edge[0])); i++)
		{
			for (int j = 0; j < 2; j++)
			{
				std::cout << edge[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//分割(特徴点の数で分割)
		cout << "特徴点の数で分割" << std::endl;
		for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
		{
			if (i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])) / 2)
			{
				for (j = 0; j < 4; j++)
				{
					divide_average1[i][j] = online[i][j];
				}
			}
			else
			{
				for (j = 0; j < 4; j++)
				{
					divide_average2[i - tokutyouten(kawari, sizeof(online) / sizeof(online[0])) / 2][j] = online[i][j];
				}
			}
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_average1[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数1" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_average1) / sizeof(divide_average1[0])) << std::endl;

		ofstream write_first("C:\\img_latest\\result_csv(average)\\" + to_string(a + 1) + "_1.csv");
		if (!write_first)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示1" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_average1) / sizeof(divide_average1[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_average1) / sizeof(divide_average1[0])))
				{
					write_first << -1 << ",";
				}
				else {
					std::cout << divide_average1[i][j] << " ";
					write_first << divide_average1[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_first << endl;
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_average2[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数2" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_average2) / sizeof(divide_average2[0])) << std::endl;

		ofstream write_second("C:\\img_latest\\result_csv(average)\\" + to_string(a + 1) + "_2.csv");
		if (!write_second)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示2" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_average2) / sizeof(divide_average2[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_average2) / sizeof(divide_average2[0]))) {
					write_second << -1 << ",";
				}
				else {
					std::cout << divide_average2[i][j] << " ";
					write_second << divide_average2[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_second << endl;
		}

		//分割(フレーム数で分割)
		cout << "フレーム数で分割" << std::endl;
		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = online[i];
		}

		int frame[4] = { online[0][2], 0, 0, 0 };	//フレーム数, 筆跡情報の番号, x座標, y座標
		for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
		{
			if (frame[0] < online[i][2])
			{
				frame[0] = online[i][2];
				frame[1] = i;
				frame[2] = online[i][0];
				frame[3] = online[i][1];
			}
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
		{
			if (i < frame[1])
			{
				for (j = 0; j < 4; j++)
				{
					divide_frame1[i][j] = online[i][j];
				}
			}
			else
			{
				for (j = 0; j < 4; j++)
				{
					divide_frame2[i - frame[1]][j] = online[i][j];
				}
			}
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_frame1[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数1()" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_frame1) / sizeof(divide_frame1[0])) << std::endl;

		ofstream write_third("C:\\img_latest\\result_csv(frame)\\" + to_string(a + 1) + "_1.csv");
		if (!write_third)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示1" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_frame1) / sizeof(divide_frame1[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_frame1) / sizeof(divide_frame1[0])))
				{
					write_third << -1 << ",";
				}
				else {
					std::cout << divide_frame1[i][j] << " ";
					write_third << divide_frame1[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_third << endl;
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_frame2[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数2" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_frame2) / sizeof(divide_frame2[0])) << std::endl;

		ofstream write_forth("C:\\img_latest\\result_csv(frame)\\" + to_string(a + 1) + "_2.csv");
		if (!write_forth)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示2" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_frame2) / sizeof(divide_frame2[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_frame2) / sizeof(divide_frame2[0]))) {
					write_forth << -1 << ",";
				}
				else {
					std::cout << divide_frame2[i][j] << " ";
					write_forth << divide_frame2[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_forth << endl;
		}

		//分割(オフラインパターンと比較して分割)
		cout << "オフラインパターンと比較して分割" << std::endl;
		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = online[i];
		}
		for (int k = 0; k < tokutyouten(kawari, sizeof(edge) / sizeof(edge[0])); k++)
		{
			if ((online[frame[1]][0] - 40 < edge[k][0] && edge[k][0] < online[frame[1]][0] + 40) && (online[frame[1]][1] - 40 < edge[k][1] && edge[k][1] < online[frame[1]][1] + 40))
			{
				//frame[1]で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1])
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1]][j] = online[i][j];
						}
					}
				}
				break;
			}
			else if ((online[frame[1] - 1][0] - 40 < edge[k][0] && edge[k][0] < online[frame[1] - 1][0] + 40) && (online[frame[1] - 1][1] - 40 < edge[k][1] && edge[k][1] < online[frame[1] - 1][1] + 40))
			{
				//frame[1] - 1で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1] - 1)
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1] - 1][j] = online[i][j];
						}
					}
				}
				break;
			}
			else if ((online[frame[1] + 1][0] - 40 < edge[k][0] && edge[k][0] < online[frame[1] + 1][0] + 40) && (online[frame[1] + 1][1] - 40 < edge[k][1] && edge[k][1] < online[frame[1] + 1][1] + 40))
			{
				//frame[1] + 1で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1] + 1)
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1] + 1][j] = online[i][j];
						}
					}
				}
				break;
			}
			/*
			else if ((online[frame[1] - 2][0] - 40 < edge[k][0] && edge[k][0] < online[frame[1] - 2][0] + 40) && (online[frame[1] - 2][1] - 40 < edge[k][1] && edge[k][1] < online[frame[1] - 2][1] + 40))
			{
				//frame[1] - 2で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1] - 2)
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1] - 2][j] = online[i][j];
						}
					}
				}
				break;
			}
			else if ((online[frame[1] + 2][0] - 40 < edge[k][0] && edge[k][0] < online[frame[1] + 2][0] + 40) && (online[frame[1] + 2][1] - 40 < edge[k][1] && edge[k][1] < online[frame[1] + 2][1] + 40))
			{
				//frame[1] + 2で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1] + 2)
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1] + 2][j] = online[i][j];
						}
					}
				}
				break;
			}
			*/
			else//フレーム近辺にないなら
			{
				//frame[1]で分割
				for (i = 0; i < tokutyouten(kawari, sizeof(online) / sizeof(online[0])); i++)
				{
					if (i < frame[1])
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare1[i][j] = online[i][j];
						}
					}
					else
					{
						for (j = 0; j < 4; j++)
						{
							divide_compare2[i - frame[1]][j] = online[i][j];
						}
					}
				}
			}
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_compare1[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数1()" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_compare1) / sizeof(divide_compare1[0])) << std::endl;

		ofstream write_fifth("C:\\img_latest\\result_csv(compare)\\" + to_string(a + 1) + "_1.csv");
		if (!write_fifth)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示1" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_compare1) / sizeof(divide_compare1[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_compare1) / sizeof(divide_compare1[0])))
				{
					write_fifth << -1 << ",";
				}
				else {
					std::cout << divide_compare1[i][j] << " ";
					write_fifth << divide_compare1[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_fifth << endl;
		}

		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_compare2[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数2" << std::endl;
		std::cout << tokutyouten(kawari, sizeof(divide_compare2) / sizeof(divide_compare2[0])) << std::endl;

		ofstream write_sixth("C:\\img_latest\\result_csv(compare)\\" + to_string(a + 1) + "_2.csv");
		if (!write_sixth)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示2" << std::endl;
		for (i = 0; i <= tokutyouten(kawari, sizeof(divide_compare2) / sizeof(divide_compare2[0])); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari, sizeof(divide_compare2) / sizeof(divide_compare2[0]))) {
					write_sixth << -1 << ",";
				}
				else {
					std::cout << divide_compare2[i][j] << " ";
					write_sixth << divide_compare2[i][j] << ",";
				}
			}
			std::cout << std::endl;
			write_sixth << endl;
		}


		//----------分離画像作成----------
		//(average)
		cv::Mat new_onlineimg1 = cv::Mat(1920, 1080, CV_8UC1);	//分割画像を1つとする画像
		cv::Mat new_onlineimg1_first = cv::Mat(1920, 1080, CV_8UC1);	//1つ目の文字画像
		cv::Mat new_onlineimg1_second = cv::Mat(1920, 1080, CV_8UC1);	//2つ目の文字画像

		new_onlineimg1 = Scalar(255);
		new_onlineimg1_first = Scalar(255);
		new_onlineimg1_second = Scalar(255);

		//分割1のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_average1[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_average1) / sizeof(divide_average1[0])) - 1; i++)
		{
			cv::line(new_onlineimg1, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg1_first, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		//分割2のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_average2[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_average2) / sizeof(divide_average2[0])) - 1; i++)
		{
			cv::line(new_onlineimg1, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg1_second, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		// 出力(bmp)
		cv::imwrite("C:\\img_latest\\result_bmp(average)\\" + to_string(a + 1) + "_1.bmp", new_onlineimg1_first);
		cv::imwrite("C:\\img_latest\\result_bmp(average)\\" + to_string(a + 1) + "_2.bmp", new_onlineimg1_second);

		//確認
		cv::imshow("Image" + to_string(a), new_onlineimg1);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE), new_onlineimg1_first);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 2), new_onlineimg1_second);

		//(frame)
		cv::Mat new_onlineimg2 = cv::Mat(1920, 1080, CV_8UC1);	//分割画像を1つとする画像
		cv::Mat new_onlineimg2_first = cv::Mat(1920, 1080, CV_8UC1);	//1つ目の文字画像
		cv::Mat new_onlineimg2_second = cv::Mat(1920, 1080, CV_8UC1);	//2つ目の文字画像

		new_onlineimg2 = Scalar(255);
		new_onlineimg2_first = Scalar(255);
		new_onlineimg2_second = Scalar(255);

		//分割1のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_frame1[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_frame1) / sizeof(divide_frame1[0])) - 1; i++)
		{
			cv::line(new_onlineimg2, cv::Point(divide_frame1[i][0], divide_frame1[i][1]), cv::Point(divide_frame1[i + 1][0], divide_frame1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg2_first, cv::Point(divide_frame1[i][0], divide_frame1[i][1]), cv::Point(divide_frame1[i + 1][0], divide_frame1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		//分割2のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_frame2[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_frame2) / sizeof(divide_frame2[0])) - 1; i++)
		{
			cv::line(new_onlineimg2, cv::Point(divide_frame2[i][0], divide_frame2[i][1]), cv::Point(divide_frame2[i + 1][0], divide_frame2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg2_second, cv::Point(divide_frame2[i][0], divide_frame2[i][1]), cv::Point(divide_frame2[i + 1][0], divide_frame2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		// 出力(bmp)
		cv::imwrite("C:\\img_latest\\result_bmp(frame)\\" + to_string(a + 1) + "_1.bmp", new_onlineimg2_first);
		cv::imwrite("C:\\img_latest\\result_bmp(frame)\\" + to_string(a + 1) + "_2.bmp", new_onlineimg2_second);

		//確認
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 4), new_onlineimg2);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 5), new_onlineimg2_first);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 6), new_onlineimg2_second);

		//(compare)
		cv::Mat new_onlineimg3 = cv::Mat(1920, 1080, CV_8UC1);	//分割画像を1つとする画像
		cv::Mat new_onlineimg3_first = cv::Mat(1920, 1080, CV_8UC1);	//1つ目の文字画像
		cv::Mat new_onlineimg3_second = cv::Mat(1920, 1080, CV_8UC1);	//2つ目の文字画像

		new_onlineimg3 = Scalar(255);
		new_onlineimg3_first = Scalar(255);
		new_onlineimg3_second = Scalar(255);

		//分割1のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_compare1[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_compare1) / sizeof(divide_compare1[0])) - 1; i++)
		{
			cv::line(new_onlineimg3, cv::Point(divide_compare1[i][0], divide_compare1[i][1]), cv::Point(divide_compare1[i + 1][0], divide_compare1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg3_first, cv::Point(divide_compare1[i][0], divide_compare1[i][1]), cv::Point(divide_compare1[i + 1][0], divide_compare1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		//分割2のパターン
		for (i = 0; i < 30; i++)
		{
			kawari[i] = divide_compare2[i];
		}
		for (i = 0; i < tokutyouten(kawari, sizeof(divide_compare2) / sizeof(divide_compare2[0])) - 1; i++)
		{
			cv::line(new_onlineimg3, cv::Point(divide_compare2[i][0], divide_compare2[i][1]), cv::Point(divide_compare2[i + 1][0], divide_compare2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg3_second, cv::Point(divide_compare2[i][0], divide_compare2[i][1]), cv::Point(divide_compare2[i + 1][0], divide_compare2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		// 出力(bmp)
		cv::imwrite("C:\\img_latest\\result_bmp(compare)\\" + to_string(a + 1) + "_1.bmp", new_onlineimg3_first);
		cv::imwrite("C:\\img_latest\\result_bmp(compare)\\" + to_string(a + 1) + "_2.bmp", new_onlineimg3_second);

		//確認
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 7), new_onlineimg3);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 8), new_onlineimg3_first);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE * 9), new_onlineimg3_second);
	}
	cv::waitKey(0);
}