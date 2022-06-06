#include <opencv2/opencv.hpp>
#include <opencv2/videostab.hpp>
#include <string>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::videostab;

// string inputPath = "inputVideo.avi";
string inputPath = "/home/guod/sensor1_2021-05-24-15_54_55.h265";
// string outputPath = "/home/guod/outputVideo.avi";
string outputPath = "/home/guod/outputsensor1SIFTVideo.h265";

void videoOutput(Ptr<IFrameSource> stabFrames, string outputPath)
{
    VideoWriter writer;
    cv::Mat stabFrame;
    
    int nframes = 0;
    double outputFps = 25;

    //traverse video frames
    while (!(stabFrame = stabFrames->nextFrame()).empty())
    {
        nframes++;

        //output video stable frame
        if (!outputPath.empty())
        {
            if (!writer.isOpened())
                writer.open(outputPath, VideoWriter::fourcc('X','V','I','D'),\
                outputFps, stabFrame.size() );
            writer<<stabFrame;
        }
        imshow("stable Frame", stabFrame);

        char key = static_cast<char>(waitKey(100));
        if ( key==27 )
        {
            cout<<endl;
            break;
        }
    }
    std::cout<< "nFrames:" << nframes << endl;
    std::cout<< "finished" << endl;
}

void cacStabVideo(Ptr<IFrameSource> stabFrames, string srcVideoFile)
{
    try
    {
        Ptr<VideoFileSource> srcVideo = makePtr<VideoFileSource>(inputPath);
        cout << "frame count:" << srcVideo->count() << endl;

        //motion estimation
        double estPara = 0.1;
        Ptr<MotionEstimatorRansacL2> est = makePtr<MotionEstimatorRansacL2>(MM_AFFINE);
        
        //Ransac parameters setting
        RansacParams ransac = est->ransacParams();
        ransac.size = 3;
        ransac.thresh = 5;
        ransac.eps = 0.5;

        //Ransac computing
        est->setRansacParams(ransac);
        est->setMinInlierRatio(estPara);

        //Fast feature detection
        //1.FAST
        // Ptr<FastFeatureDetector> feature_detector = FastFeatureDetector::create();
        //2.SIFT
        //SIFT feature detection
        Ptr<SiftFeatureDetector> feature_detector = SiftFeatureDetector::create();
        //3.ORB
        Ptr<>



        //Motion estimation key point matching
        Ptr<KeypointBasedMotionEstimator> motionEstBuilder = makePtr<KeypointBasedMotionEstimator>(est);

        //set feature detector
        motionEstBuilder->setDetector(feature_detector);
        Ptr<IOutlierRejector> outlierRejector = makePtr<NullOutlierRejector>();
        motionEstBuilder->setOutlierRejector(outlierRejector);

        //3-Prepare the stabilizer
        StabilizerBase *stabilizer = 0;
        //first, prepare the one or two pass stabilizer
        bool isTwoPass = 1;
        int radius_pass = 15;
        if (isTwoPass)
        {
            //with a two pass stabilizer
            bool est_trim = true;
            TwoPassStabilizer *twoPassStabilizer = new TwoPassStabilizer();
            twoPassStabilizer->setEstimateTrimRatio(est_trim);
            twoPassStabilizer->setMotionStabilizer( makePtr<GaussianMotionFilter>(radius_pass));
            stabilizer = twoPassStabilizer;
        }
        else
        {
            //with an one pass stabilizer
            OnePassStabilizer *onePassStabilizer = new OnePassStabilizer();
            onePassStabilizer->setMotionFilter( makePtr<GaussianMotionFilter>(radius_pass));
            stabilizer = onePassStabilizer;
        }

        //second, set up the parameters
        int radius = 15;
        double trim_ratio = 0.1;
        bool incl_constr = false;

        stabilizer->setFrameSource(srcVideo);
        stabilizer->setMotionEstimator(motionEstBuilder);
        
        stabilizer->setRadius(radius);
        stabilizer->setTrimRatio(trim_ratio);
        stabilizer->setCorrectionForInclusion(incl_constr);
        stabilizer->setBorderMode(BORDER_REPLICATE);
        stabFrames.reset( dynamic_cast<IFrameSource*>(stabilizer) );
        videoOutput(stabFrames, outputPath);
    }

    catch (const exception &e)
    {
        cout << "error:" << e.what() << endl;
        stabFrames.release();
    }
}

int main( int argc, char* argv[] )
{
    Ptr<IFrameSource> stabFrames;

    cacStabVideo(stabFrames, inputPath);
    stabFrames.release();

    return 0;
}
