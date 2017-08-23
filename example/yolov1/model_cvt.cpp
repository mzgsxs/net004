#include "Net004.h"
#include "DataLayer.h"
#include "Parser.h"
#include "caffe_parser.h"
#include "opencv2/opencv.hpp"
#include "glog/logging.h"
#include <map>
using namespace std;
using namespace cv;

void cvt_caffe_model(){
	string caffe_net_path, caffe_model_path, net004_net_path, net004_model_path;
	map<string,vector<string> > maps;

	caffe_net_path = "../caffe_models/detection/gnet_deploy.prototxt";
	caffe_model_path = "../caffe_models/detection/gnet_yolo_iter_32000.caffemodel";
	net004_net_path = "../models/detection/yolov1.net004.net";
	net004_model_path = "../models/detection/yolov1.net004.data";
	maps["yolov1"] = {caffe_net_path, caffe_model_path,net004_net_path,net004_model_path};
	string name = "yolov1";

	printf("convert caffe model:\n");
	printf("src net: %s\n",maps[name][0].c_str());
	printf("src model: %s\n",maps[name][1].c_str());
	printf("des net: %s\n",maps[name][2].c_str());
	printf("des model: %s\n",maps[name][3].c_str());
	CaffeModelParser parser;
	parser.load_caffe_model(maps[name][0], maps[name][1]);
	parser.write(maps[name][2], maps[name][3]);

}
int main(int argc, char **argv){
	google::InitGoogleLogging(argv[0]);
	google::SetCommandLineOption("GLOG_minloglevel", "2");

	cvt_caffe_model();
	return 0;
}