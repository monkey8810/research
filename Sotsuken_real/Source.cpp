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

//特徴点のカウントメソッド
int tokutyouten(int **data)
{
	for (int i = 0; i < 30; i++)
	{
		if (data[i][0] == 0)
		{
			return i;
		}
	}
	return 30;
}

// 細線化メソッド
void thinningIte(cv::Mat& img, int pattern) {

	cv::Mat del_marker = cv::Mat::ones(img.size(), CV_8UC1);
	int x, y;

	for (y = 1; y < img.rows - 1; ++y) {

		for (x = 1; x < img.cols - 1; ++x) {

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

	do {
		thinningIte(dst, 0);
		thinningIte(dst, 1);
		absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (countNonZero(diff) > 0);

	dst *= 255;
}

int main(void)
{
	// 連番読み込み
	/*
	int src_size = 0;
	Mat search_img[MAX_IMAGESIZE];

	for (int i = 0; i < MAX_IMAGESIZE;i++)
	{
		search_img[i] = imread("C:\\img\\online\\"+ to_string(i + 1) + ".bmp", 1);
		if (!search_img[i].data) break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		src_size++;
	}
	for (int i = 0; i < src_size; i++) {
		imshow("search_" + to_string(i + 1), search_img[i]); //入力された画像を表示するウィンドウを自動で生成
	}
	cv::waitKey(0);
	*/

	//オンライン読み込み
	cv::Mat src_img_off = cv::imread("C:\\2017data\\CheckCheck\\kakiwari(01).bmp", 1);
	cv::Mat src_img_off2;	//細線化用
	if (src_img_off.empty()) return -1;

	//リサイズ(Lanczos法の補間)
	//resize(src_img_off, src_img_off, cv::Size(), 720.0 / src_img_off.cols, 1280.0 / src_img_off.rows, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5);
	std::cout << "width_off: " << src_img_off.cols << std::endl;
	std::cout << "height_off: " << src_img_off.rows << std::endl;

	//グレー画像化
	cv::cvtColor(src_img_off, src_img_off, CV_RGB2GRAY);

	//2値化
	cv::threshold(src_img_off, src_img_off2, 1, 255, CV_THRESH_BINARY);

	// 白黒反転
	bitwise_not(src_img_off2, src_img_off2);

	// 細線化
	thinning(src_img_off2, src_img_off2);

	// 白黒反転
	bitwise_not(src_img_off2, src_img_off2);

	for (int y = 0; y < src_img_off2.rows; y++) {
		for (int x = 0; x < src_img_off2.cols; x++) {
			//X座標がx, Y座標がyに位置するピクセルの値を取得
			int intensity = src_img_off2.at<unsigned char>(y, x);
			if (intensity == 0 && (x == 508 || y == 1070)) {

				printf("(%d,%d)", x, y);
			}
		}
		printf("");
	}

	//画像表示
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("Image1", src_img_off);
	cv::imshow("Image2", src_img_off2);


	/*
	//csvファイル読み込み(連番)
	int i, j;

	for (int a = 0; a < MAX_IMAGESIZE; a++)
	{
		int divide_average1[15][4] = { 0 };
		int divide_average2[15][4] = { 0 };
		int online[30][4] = { 0 };	//x,y,フレーム数,方向角
		int *kawari[30];

		ifstream file("C:\\img\\csv\\" + to_string(a + 1) + ".csv");
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
					online[i][j] = online[i][j] / 1.5;
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
		std::cout << tokutyouten(kawari) << std::endl;

		cout << "特徴点のみ表示" << std::endl;
		for (i = 0; i < tokutyouten(kawari); i++)
		{
			for (j = 0; j < 4; j++)
			{
				std::cout << online[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//分割
		for (i = 0; i < tokutyouten(kawari); i++)
		{
			if (i < tokutyouten(kawari) / 2) {
				for (j = 0; j < 4; j++)
				{
					divide_average1[i][j] = online[i][j];
				}
			}
			else {
				for (j = 0; j < 4; j++)
				{
					divide_average2[i - tokutyouten(kawari) / 2][j] = online[i][j];
				}
			}
		}

		//確認
		//2次元配列の受け渡し用入れ替え
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average1[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数1" << std::endl;
		std::cout << tokutyouten(kawari) << std::endl;

		ofstream write_first("C:\\img\\result_csv\\" + to_string(a + 1) + "_1.csv");
		if (!write_first)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示1" << std::endl;
		for (i = 0; i <= tokutyouten(kawari); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari))
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
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average2[i];
		}

		//特徴点の数確認 & 出力(csv)
		cout << "分割した特徴点の数2" << std::endl;
		std::cout << tokutyouten(kawari) << std::endl;

		ofstream write_second("C:\\img\\result_csv\\" + to_string(a + 1) + "_2.csv");
		if (!write_second)
		{
			cout << "入力エラー" << std::endl;
			return 1;
		}

		cout << "分割した特徴点表示2" << std::endl;
		for (i = 0; i <= tokutyouten(kawari); i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (i == tokutyouten(kawari)) {
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

		//----------分離画像作成----------
		cv::Mat new_onlineimg = cv::Mat(1280, 720, CV_8UC1);	//分割画像を1つとする画像
		cv::Mat new_onlineimg_first = cv::Mat(1280, 720, CV_8UC1);	//1つ目の文字画像
		cv::Mat new_onlineimg2_second = cv::Mat(1280, 720, CV_8UC1);	//2つ目の文字画像

		new_onlineimg = Scalar(255);
		new_onlineimg_first = Scalar(255);
		new_onlineimg2_second = Scalar(255);

		//分割1のパターン
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average1[i];
		}
		for (i = 0; i < tokutyouten(kawari) - 1; i++)
		{
			cv::line(new_onlineimg, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg_first, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		//分割2のパターン
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average2[i];
		}
		for (i = 0; i < tokutyouten(kawari) - 1; i++)
		{
			cv::line(new_onlineimg, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg2_second, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		// 出力(bmp)
		cv::imwrite("C:\\img\\result_bmp\\" + to_string(a + 1) + "_1.bmp", new_onlineimg_first);
		cv::imwrite("C:\\img\\result_bmp\\" + to_string(a + 1) + "_2.bmp", new_onlineimg2_second);

		//確認
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE), new_onlineimg);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 1), new_onlineimg_first);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 2), new_onlineimg2_second);
	}




	*/
	cv::waitKey(0);
	return 0;
}