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

	//�I�t���C���ǂݍ���
	cv::Mat src_img_off = cv::imread("C:\\2017data\\CheckCheck\\Symbol_Pattern(01).bmp", 1);
	if (src_img_off.empty()) return -1;

	//���T�C�Y
	resize(src_img_off, src_img_off, cv::Size(), 720.0 / src_img_off.cols, 1280.0 / src_img_off.rows);
	std::cout << "width_off: " << src_img_off.cols << std::endl;
	std::cout << "height_off: " << src_img_off.rows << std::endl;

	//�O���[�摜��
	cv::cvtColor(src_img_off, src_img_off, CV_RGB2GRAY);

	//2�l��
	//cv::threshold(src_img_off, src_img_off, 0, 255, CV_THRESH_BINARY);

	//�摜�\��
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
	cv::imshow("Image1", src_img_off);

	//csv�t�@�C���ǂݍ���(�A��)
	int i, j;

	for (int a = 0; a < MAX_IMAGESIZE; a++)
	{
		int divide_average1[15][4] = { 0 };
		int divide_average2[15][4] = { 0 };
		int online[30][4] = { 0 };
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
		//cv::imshow("Image" + to_string(a + MAX_IMAGESIZE), new_onlineimg);
		//cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 1), new_onlineimg_first);
		//cv::imshow("Image" + to_string(a + MAX_IMAGESIZE + 2), new_onlineimg2_second);
	}
	cv::waitKey(0);
	return 0;
}