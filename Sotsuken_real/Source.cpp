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
	int count = 0;
	for (int i = 0; i < 30; i++)
	{
		if (data[i][0] == 0)
		{
			return count;
		}
		count++;
	}
	return 30;
}

int main(void)
{
	//char imagename[50];
	//char filename[50];


	//for (int number = 1; number <= 16; number++)
	//{
	//sprintf(imagename, "C:\\2017data\\%d\\kakiwari(01).bmp", number);

	//オフライン読み込み
	cv::Mat src_img_off = cv::imread("C:\\2017data\\CheckCheck\\Symbol_Pattern(01).bmp", 1);
	if (src_img_off.empty()) return -1;

	//リサイズ
	resize(src_img_off, src_img_off, cv::Size(), 720.0 / src_img_off.cols, 1280.0 / src_img_off.rows);
	std::cout << "width_off: " << src_img_off.cols << std::endl;
	std::cout << "height_off: " << src_img_off.rows << std::endl;

	//グレー画像化
	cv::cvtColor(src_img_off, src_img_off, CV_RGB2GRAY);

	//2値化
	//cv::threshold(src_img_off, src_img_off, 0, 255, CV_THRESH_BINARY);

	//画像表示
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("Image1", src_img_off);

	//sprintf(filename, "C:\\2017data\\CheckCheck\\CheckCheck.csv", number, number);

	ifstream file("C:\\2017data\\CheckCheck\\CheckCheck.csv");
	//ifstream file("C:\\2017data\\a.csv");
	string str;
	string token;
	int i, j;
	int divide_average1[15][4] = { 0 };
	int divide_average2[15][4] = { 0 };
	int online[30][4] = { 0 };
	int *kawari[30];

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

	//特徴点の数確認
	cout << "分割した特徴点の数1" << std::endl;
	std::cout << tokutyouten(kawari) << std::endl;

	cout << "分割した特徴点表示1" << std::endl;
	for (i = 0; i < tokutyouten(kawari); i++)
	{
		for (j = 0; j < 4; j++)
		{
			std::cout << divide_average1[i][j] << " ";
		}
		std::cout << std::endl;
	}

	//2次元配列の受け渡し用入れ替え
	for (i = 0; i < 15; i++)
	{
		kawari[i] = divide_average2[i];
	}

	//特徴点の数確認
	cout << "分割した特徴点の数2" << std::endl;
	std::cout << tokutyouten(kawari) << std::endl;

	cout << "分割した特徴点表示2" << std::endl;
	for (i = 0; i < tokutyouten(kawari); i++)
	{
		for (j = 0; j < 4; j++)
		{
			std::cout << divide_average2[i][j] << " ";
		}
		std::cout << std::endl;
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

	//確認
	cv::imshow("Image2", new_onlineimg);
	cv::imshow("Image3", new_onlineimg_first);
	cv::imshow("Image4", new_onlineimg2_second);

	cv::waitKey(0);
	//}
	return 0;
}