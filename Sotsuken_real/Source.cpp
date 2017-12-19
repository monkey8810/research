// �����J�[�̓��͂�ݒ�
// �f�o�b�O�ƃ����[�X�œ��͂���t�@�C�����قȂ�̂ł��̂悤�ɂ��Ă��܂��B
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

#define MAX_IMAGESIZE 16 // �ǂݍ��݉摜��

//�p�X C:\\2017data\\CheckCheck\\CheckCheck.csv

//�����_�̃J�E���g���\�b�h
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

// �א������\�b�h
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
	dst /= 255;         // 0��0 , 1�ȏ��1�ɕϊ������

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
	// �A�ԓǂݍ���
	/*
	int src_size = 0;
	Mat search_img[MAX_IMAGESIZE];

	for (int i = 0; i < MAX_IMAGESIZE;i++)
	{
		search_img[i] = imread("C:\\img\\online\\"+ to_string(i + 1) + ".bmp", 1);
		if (!search_img[i].data) break;	//�S�Ẳ摜��(�A�Ԃ�)�ǂݍ��ݏI����ƃ��[�v�𔲂���
		src_size++;
	}
	for (int i = 0; i < src_size; i++) {
		imshow("search_" + to_string(i + 1), search_img[i]); //���͂��ꂽ�摜��\������E�B���h�E�������Ő���
	}
	cv::waitKey(0);
	*/

	//�I�����C���ǂݍ���
	cv::Mat src_img_off = cv::imread("C:\\2017data\\CheckCheck\\kakiwari(01).bmp", 1);
	cv::Mat src_img_off2;	//�א����p
	if (src_img_off.empty()) return -1;

	//���T�C�Y(Lanczos�@�̕��)
	//resize(src_img_off, src_img_off, cv::Size(), 720.0 / src_img_off.cols, 1280.0 / src_img_off.rows, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5, cv::INTER_LANCZOS4);
	//resize(src_img_off, src_img_off, cv::Size(), 0.5, 0.5);
	std::cout << "width_off: " << src_img_off.cols << std::endl;
	std::cout << "height_off: " << src_img_off.rows << std::endl;

	//�O���[�摜��
	cv::cvtColor(src_img_off, src_img_off, CV_RGB2GRAY);

	//2�l��
	cv::threshold(src_img_off, src_img_off2, 1, 255, CV_THRESH_BINARY);

	// �������]
	bitwise_not(src_img_off2, src_img_off2);

	// �א���
	thinning(src_img_off2, src_img_off2);

	// �������]
	bitwise_not(src_img_off2, src_img_off2);

	for (int y = 0; y < src_img_off2.rows; y++) {
		for (int x = 0; x < src_img_off2.cols; x++) {
			//X���W��x, Y���W��y�Ɉʒu����s�N�Z���̒l���擾
			int intensity = src_img_off2.at<unsigned char>(y, x);
			if (intensity == 0 && (x == 508 || y == 1070)) {

				printf("(%d,%d)", x, y);
			}
		}
		printf("");
	}

	//�摜�\��
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("Image1", src_img_off);
	cv::imshow("Image2", src_img_off2);


	/*
	//csv�t�@�C���ǂݍ���(�A��)
	int i, j;

	for (int a = 0; a < MAX_IMAGESIZE; a++)
	{
		int divide_average1[15][4] = { 0 };
		int divide_average2[15][4] = { 0 };
		int online[30][4] = { 0 };	//x,y,�t���[����,�����p
		int *kawari[30];

		ifstream file("C:\\img\\csv\\" + to_string(a + 1) + ".csv");
		string str;
		string token;

		//�ǂ߂Ȃ������Ƃ�
		if (!file)
		{
			cout << "���̓G���[" << std::endl;
			return 1;
		}

		cout << "csv�t�@�C���̒��g" << std::endl;
		i = 0;
		//csv�t�@�C����1�s���ǂݍ���
		while (getline(file, str))
		{
			j = 0;
			istringstream stream(str);
			//1�s�̂����A������ƃR���}�𕪊�����
			while (getline(stream, token, ','))
			{
				//���ׂĕ�����Ƃ��ēǂݍ��܂�邽��
				//���l�͕ϊ����K�v
				online[i][j] = stoi(token);
				cout << online[i][j] << ",";
				//�T�C�Y���킹
				if (j == 0 || j == 1) {
					online[i][j] = online[i][j] / 1.5;
				}
				j++;
			}
			cout << endl;
			i++;
		}
		//�m�F
		cout << "csv�t�@�C�����ڂ����z��̒��g" << std::endl;
		for (i = 0; i < 30; i++)
		{
			for (j = 0; j < 4; j++)
			{
				std::cout << online[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//2�����z��̎󂯓n���p����ւ�
		for (i = 0; i < 30; i++)
		{
			kawari[i] = online[i];
		}

		//�����_�̐��m�F
		cout << "�����_�̐�" << std::endl;
		std::cout << tokutyouten(kawari) << std::endl;

		cout << "�����_�̂ݕ\��" << std::endl;
		for (i = 0; i < tokutyouten(kawari); i++)
		{
			for (j = 0; j < 4; j++)
			{
				std::cout << online[i][j] << " ";
			}
			std::cout << std::endl;
		}

		//����
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

		//�m�F
		//2�����z��̎󂯓n���p����ւ�
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average1[i];
		}

		//�����_�̐��m�F & �o��(csv)
		cout << "�������������_�̐�1" << std::endl;
		std::cout << tokutyouten(kawari) << std::endl;

		ofstream write_first("C:\\img\\result_csv\\" + to_string(a + 1) + "_1.csv");
		if (!write_first)
		{
			cout << "���̓G���[" << std::endl;
			return 1;
		}

		cout << "�������������_�\��1" << std::endl;
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

		//2�����z��̎󂯓n���p����ւ�
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average2[i];
		}

		//�����_�̐��m�F & �o��(csv)
		cout << "�������������_�̐�2" << std::endl;
		std::cout << tokutyouten(kawari) << std::endl;

		ofstream write_second("C:\\img\\result_csv\\" + to_string(a + 1) + "_2.csv");
		if (!write_second)
		{
			cout << "���̓G���[" << std::endl;
			return 1;
		}

		cout << "�������������_�\��2" << std::endl;
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

		//----------�����摜�쐬----------
		cv::Mat new_onlineimg = cv::Mat(1280, 720, CV_8UC1);	//�����摜��1�Ƃ���摜
		cv::Mat new_onlineimg_first = cv::Mat(1280, 720, CV_8UC1);	//1�ڂ̕����摜
		cv::Mat new_onlineimg2_second = cv::Mat(1280, 720, CV_8UC1);	//2�ڂ̕����摜

		new_onlineimg = Scalar(255);
		new_onlineimg_first = Scalar(255);
		new_onlineimg2_second = Scalar(255);

		//����1�̃p�^�[��
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average1[i];
		}
		for (i = 0; i < tokutyouten(kawari) - 1; i++)
		{
			cv::line(new_onlineimg, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg_first, cv::Point(divide_average1[i][0], divide_average1[i][1]), cv::Point(divide_average1[i + 1][0], divide_average1[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		//����2�̃p�^�[��
		for (i = 0; i < 15; i++)
		{
			kawari[i] = divide_average2[i];
		}
		for (i = 0; i < tokutyouten(kawari) - 1; i++)
		{
			cv::line(new_onlineimg, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
			cv::line(new_onlineimg2_second, cv::Point(divide_average2[i][0], divide_average2[i][1]), cv::Point(divide_average2[i + 1][0], divide_average2[i + 1][1]), cv::Scalar(0), 1, CV_AA);
		}

		// �o��(bmp)
		cv::imwrite("C:\\img\\result_bmp\\" + to_string(a + 1) + "_1.bmp", new_onlineimg_first);
		cv::imwrite("C:\\img\\result_bmp\\" + to_string(a + 1) + "_2.bmp", new_onlineimg2_second);

		//�m�F
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE), new_onlineimg);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 1), new_onlineimg_first);
		cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 2), new_onlineimg2_second);
	}




	*/
	cv::waitKey(0);
	return 0;
}