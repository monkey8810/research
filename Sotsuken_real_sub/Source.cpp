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

#define OFF_CHECK 248
#define OFF_CIRCLE 250
#define OFF_CROSS 249
#define OFF_TRIANGLE 250
#define ON_CHECK 334
#define ON_CIRCLE 280
#define ON_CROSS 324
#define ON_TRIANGLE 331
#define _CRT_SECURE_NO_WARNINGS

//ヒストグラム画像作成
void make_histogram_image(cv::MatND& hist, cv::Mat& hist_img, int bin_num)
{
	// histogramを描画するための画像領域を確保
	int img_width = 512;
	int img_height = 512;
	hist_img = cv::Mat(cv::Size(img_width, img_height), CV_8UC3);

	// ヒストグラムのスケーリング
	// ヒストグラムのbinの中で、頻度数最大のbinの高さが、ちょうど画像の縦幅と同じ値になるようにする
	double max_val = 0.0;
	cv::minMaxLoc(hist, 0, &max_val);
	hist = hist * (max_val ? img_height / max_val : 0.0);

	// ヒストグラムのbinの数だけ矩形を書く
	for (int j = 0; j < bin_num; ++j) {
		// saturate_castは、安全に型変換するための関数。桁あふれを防止
		int bin_w = cv::saturate_cast<int>((double)img_width / bin_num);
		cv::rectangle(
			hist_img,
			cv::Point(j*bin_w, hist_img.rows),
			cv::Point((j + 1)*bin_w, hist_img.rows - cv::saturate_cast<int>(hist.at<float>(j))),
			cv::Scalar::all(0), -1);
	}
}

int main(void)
{
	// 連番読み込み(オフライン画像)
	int off_check_size = 0;
	int off_circle_size = 0;
	int off_cross_size = 0;
	int off_triangle_size = 0;
	int on_check_size = 0;
	int on_circle_size = 0;
	int on_cross_size = 0;
	int on_triangle_size = 0;
	Mat off_check[OFF_CHECK];
	Mat off_circle[OFF_CIRCLE];
	Mat off_cross[OFF_CROSS];
	Mat off_triangle[OFF_TRIANGLE];
	Mat on_check[ON_CHECK];
	Mat on_circle[ON_CIRCLE];
	Mat on_cross[ON_CROSS];
	Mat on_triangle[ON_TRIANGLE];
	Mat a;
	Mat teach_off_check;
	Mat teach_off_circle;
	Mat teach_off_cross;
	Mat teach_off_triangle;
	Mat teach_on_check;
	Mat teach_on_circle;
	Mat teach_on_cross;
	Mat teach_on_triangle;

	//教師データ
	for (int i = 0; i < OFF_CHECK; i++)
	{
		off_check[i] = imread("C:\\teachdata\\offlinepattern\\camera_finnish_glayscale\\studyset\\check\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!off_check[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(off_check[i], off_check[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		off_check[i].convertTo(a, CV_64FC3);
		teach_off_check = a + teach_off_check;
		off_check_size++;
	}
	teach_off_check = teach_off_check / off_check_size;
	teach_off_check.convertTo(teach_off_check, CV_8UC3);
	//2値化
	//threshold(teach_off_check, teach_off_check, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_off_check.bmp", teach_off_check);

	for (int i = 0; i < OFF_CIRCLE; i++)
	{
		off_circle[i] = imread("C:\\teachdata\\offlinepattern\\camera_finnish_glayscale\\studyset\\circle\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!off_circle[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(off_circle[i], off_circle[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		off_circle[i].convertTo(a, CV_64FC3);
		teach_off_circle = a + teach_off_circle;
		off_circle_size++;
	}
	teach_off_circle = teach_off_circle / off_circle_size;
	teach_off_circle.convertTo(teach_off_circle, CV_8UC3);
	//2値化
	//threshold(teach_off_circle, teach_off_circle, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_off_circle.bmp", teach_off_circle);

	for (int i = 0; i < OFF_CROSS; i++)
	{
		off_cross[i] = imread("C:\\teachdata\\offlinepattern\\camera_finnish_glayscale\\studyset\\cross\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!off_cross[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(off_cross[i], off_cross[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		off_cross[i].convertTo(a, CV_64FC3);
		teach_off_cross = a + teach_off_cross;
		off_cross_size++;
	}
	teach_off_cross = teach_off_cross / off_cross_size;
	teach_off_cross.convertTo(teach_off_cross, CV_8UC3);
	//2値化
	//threshold(teach_off_cross, teach_off_cross, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_off_cross.bmp", teach_off_cross);

	for (int i = 0; i < OFF_TRIANGLE; i++)
	{
		off_triangle[i] = imread("C:\\teachdata\\offlinepattern\\camera_finnish_glayscale\\studyset\\triangle\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!off_triangle[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(off_triangle[i], off_triangle[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		off_triangle[i].convertTo(a, CV_64FC3);
		teach_off_triangle = a + teach_off_triangle;
		off_triangle_size++;
	}
	teach_off_triangle = teach_off_triangle / off_triangle_size;
	teach_off_triangle.convertTo(teach_off_triangle, CV_8UC3);
	//2値化
	//threshold(teach_off_triangle, teach_off_triangle, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_off_triangle.bmp", teach_off_triangle);

	for (int i = 0; i < ON_CHECK; i++)
	{
		on_check[i] = imread("C:\\teachdata\\onlinepattern\\StudyPat\\check\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!on_check[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(on_check[i], on_check[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		on_check[i].convertTo(a, CV_64FC3);
		teach_on_check = a + teach_on_check;
		on_check_size++;
	}
	teach_on_check = teach_on_check / on_check_size;
	teach_on_check.convertTo(teach_on_check, CV_8UC3);
	//2値化
	//threshold(teach_on_check, teach_on_check, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_on_check.bmp", teach_on_check);

	for (int i = 0; i < ON_CIRCLE; i++)
	{
		on_circle[i] = imread("C:\\teachdata\\onlinepattern\\StudyPat\\circle\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!on_circle[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(on_circle[i], on_circle[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		on_circle[i].convertTo(a, CV_64FC3);
		teach_on_circle = a + teach_on_circle;
		on_circle_size++;
	}
	teach_on_circle = teach_on_circle / on_circle_size;
	teach_on_circle.convertTo(teach_on_circle, CV_8UC3);
	//2値化
	//threshold(teach_on_circle, teach_on_circle, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_on_circle.bmp", teach_on_circle);

	for (int i = 0; i < ON_CROSS; i++)
	{
		on_cross[i] = imread("C:\\teachdata\\onlinepattern\\StudyPat\\cross\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!on_cross[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(on_cross[i], on_cross[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		on_cross[i].convertTo(a, CV_64FC3);
		teach_on_cross = a + teach_on_cross;
		on_cross_size++;
	}
	teach_on_cross = teach_on_cross / on_cross_size;
	teach_on_cross.convertTo(teach_on_cross, CV_8UC3);
	//2値化
	//threshold(teach_on_cross, teach_on_cross, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_on_cross.bmp", teach_on_cross);

	for (int i = 0; i < ON_TRIANGLE; i++)
	{
		on_triangle[i] = imread("C:\\teachdata\\onlinepattern\\StudyPat\\triangle\\ (" + to_string(i + 1) + ").bmp", IMREAD_GRAYSCALE);
		if (!on_triangle[i].data)
		{
			break;	//全ての画像を(連番で)読み込み終えるとループを抜ける
		}
		//2値化
		threshold(on_triangle[i], on_triangle[i], 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
		on_triangle[i].convertTo(a, CV_64FC3);
		teach_on_triangle = a + teach_on_triangle;
		on_triangle_size++;
	}
	teach_on_triangle = teach_on_triangle / on_triangle_size;
	teach_on_triangle.convertTo(teach_on_triangle, CV_8UC3);
	//2値化
	//threshold(teach_on_triangle, teach_on_triangle, 0, 255, THRESH_BINARY | THRESH_OTSU); //閾値を自動で設定
	imwrite("C:\\result_forout\\teach_on_triangle.bmp", teach_on_triangle);

	//テストデータ
	Mat divide_check[8];
	Mat divide_circle[8];
	Mat divide_cross[8];
	Mat divide_triangle[8];

	//()の中身をaverage,average,comoareに変更して使用
	for (int i = 0; i < 16; i++)
	{
		if (i < 4)
		{
			divide_check[i] = imread("C:\\img_latest\\result_bmp(compare)\\" + to_string(i + 1) + "_1.bmp", 1);
		}
		else if (i < 8)
		{
			divide_circle[i - 4] = imread("C:\\img_latest\\result_bmp(compare)\\" + to_string(i + 1) + "_1.bmp", 1);
		}
		else if (i < 12)
		{
			divide_cross[i - 8] = imread("C:\\img_latest\\result_bmp(compare)\\" + to_string(i + 1) + "_1.bmp", 1);
		}
		else
		{
			divide_triangle[i - 12] = imread("C:\\img_latest\\result_bmp(compare)\\" + to_string(i + 1) + "_1.bmp", 1);
		}
	}
	
	divide_check[4] = imread("C:\\img_latest\\result_bmp(compare)\\1_2.bmp", 1);
	divide_check[5] = imread("C:\\img_latest\\result_bmp(compare)\\5_2.bmp", 1);
	divide_check[6] = imread("C:\\img_latest\\result_bmp(compare)\\9_2.bmp", 1);
	divide_check[7] = imread("C:\\img_latest\\result_bmp(compare)\\13_2.bmp", 1);
	divide_circle[4] = imread("C:\\img_latest\\result_bmp(compare)\\2_2.bmp", 1);
	divide_circle[5] = imread("C:\\img_latest\\result_bmp(compare)\\6_2.bmp", 1);
	divide_circle[6] = imread("C:\\img_latest\\result_bmp(compare)\\10_2.bmp", 1);
	divide_circle[7] = imread("C:\\img_latest\\result_bmp(compare)\\14_2.bmp", 1);
	divide_cross[4] = imread("C:\\img_latest\\result_bmp(compare)\\3_2.bmp", 1);
	divide_cross[5] = imread("C:\\img_latest\\result_bmp(compare)\\7_2.bmp", 1);
	divide_cross[6] = imread("C:\\img_latest\\result_bmp(compare)\\11_2.bmp", 1);
	divide_cross[7] = imread("C:\\img_latest\\result_bmp(compare)\\15_2.bmp", 1);
	divide_triangle[4] = imread("C:\\img_latest\\result_bmp(compare)\\4_2.bmp", 1);
	divide_triangle[5] = imread("C:\\img_latest\\result_bmp(compare)\\8_2.bmp", 1);
	divide_triangle[6] = imread("C:\\img_latest\\result_bmp(compare)\\12_2.bmp", 1);
	divide_triangle[7] = imread("C:\\img_latest\\result_bmp(compare)\\16_2.bmp", 1);

	//膨張演算
	for (int i = 0; i < 8; i++)
	{
		erode(divide_check[i], divide_check[i], Mat(), Point(-1, -1), 10);
		erode(divide_circle[i], divide_circle[i], Mat(), Point(-1, -1), 10);
		erode(divide_cross[i], divide_cross[i], Mat(), Point(-1, -1), 10);
		erode(divide_triangle[i], divide_triangle[i], Mat(), Point(-1, -1), 10);
	}

	//類似度を算出
	// ヒストグラムを生成するために必要なデータ
	cv::MatND teach_hist[8];         // ここにヒストグラムが出力される	教師データ
	cv::MatND evaluate_hist[32];         // ここにヒストグラムが出力される	検証データ
	int image_num = 1;      // 入力画像の枚数
	int channels[] = { 0 }; // cv::Matの何番目のチャネルを使うか　今回は白黒画像なので0番目のチャネル以外選択肢なし
	int dim_num = 1;        // ヒストグラムの次元数
	int bin_num = 64;       // ヒストグラムのビンの数
	int bin_nums[] = { bin_num };      // 今回は1次元のヒストグラムを作るので要素数は一つ
	float range[] = { 0, 256 };        // 扱うデータの最小値、最大値　今回は輝度データなので値域は[0, 255]
	const float *ranges[] = { range }; // 今回は1次元のヒストグラムを作るので要素数は一つ

	//教師データのヒストグラムを求める
	//白黒画像から輝度のヒストグラムデータ（＝各binごとの出現回数をカウントしたもの）を生成
	cv::calcHist(&teach_off_check, image_num, channels, cv::Mat(), teach_hist[0], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_off_circle, image_num, channels, cv::Mat(), teach_hist[1], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_off_cross, image_num, channels, cv::Mat(), teach_hist[2], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_off_triangle, image_num, channels, cv::Mat(), teach_hist[3], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_on_check, image_num, channels, cv::Mat(), teach_hist[4], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_on_circle, image_num, channels, cv::Mat(), teach_hist[5], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_on_cross, image_num, channels, cv::Mat(), teach_hist[6], dim_num, bin_nums, ranges);
	cv::calcHist(&teach_on_triangle, image_num, channels, cv::Mat(), teach_hist[7], dim_num, bin_nums, ranges);

	//検証データのヒストグラムを求める
	for (int i = 0; i < 8; i++)
	{
		//白黒画像から輝度のヒストグラムデータ（＝各binごとの出現回数をカウントしたもの）を生成
		cv::calcHist(&divide_check[i], image_num, channels, cv::Mat(), evaluate_hist[i], dim_num, bin_nums, ranges);
		cv::calcHist(&divide_circle[i], image_num, channels, cv::Mat(), evaluate_hist[i + 8], dim_num, bin_nums, ranges);
		cv::calcHist(&divide_cross[i], image_num, channels, cv::Mat(), evaluate_hist[i + 16], dim_num, bin_nums, ranges);
		cv::calcHist(&divide_triangle[i], image_num, channels, cv::Mat(), evaluate_hist[i + 24], dim_num, bin_nums, ranges);
	}

	// テキスト形式でヒストグラムデータを確認
	//std::cout << hist << std::endl;

	// ヒストグラムデータを表示用の画像に変換
	cv::Mat hist_img;
	/*
	// OpenCVでは関数が用意されていないので自前で用意する必要がある
	make_histogram_image(hist[0], hist_img, bin_num);
	cv::imshow("histogram image1", hist_img);


	cv::calcHist(&teach_off_triangle, image_num, channels, cv::Mat(), hist[1], dim_num, bin_nums, ranges);
	// テキスト形式でヒストグラムデータを確認
	std::cout << hist << std::endl;

	// ヒストグラムデータを表示用の画像に変換
	// OpenCVでは関数が用意されていないので自前で用意する必要がある
	make_histogram_image(hist[1], hist_img, bin_num);
	cv::imshow("histogram image2", hist_img);
	*/

	double correl;
	for (int i = 0;i < 32;i++)
	{
		if (i < 8)
		{
			correl = compareHist(teach_hist[0], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(check_off):" << correl << std::endl;
			correl = compareHist(teach_hist[4], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(check_on):" << correl << std::endl;
		}
		else if (i < 16)
		{
			correl = compareHist(teach_hist[1], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(circle_off):" << correl << std::endl;
			correl = compareHist(teach_hist[5], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(circle_on):" << correl << std::endl;
		}
		else if (i < 24)
		{
			correl = compareHist(teach_hist[2], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(cross_off):" << correl << std::endl;
			correl = compareHist(teach_hist[6], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(cross_on):" << correl << std::endl;
		}
		else
		{
			correl = compareHist(teach_hist[3], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(triangle_off):" << correl << std::endl;
			correl = compareHist(teach_hist[7], evaluate_hist[i], CV_COMP_CORREL);
			std::cout << "correl(triangle_on):" << correl << std::endl;
		}
	}

	cv::waitKey(0);
	getchar();
}