import cv2 

if __name__ == "__main__":
    path1 = '/home/guod/VSCodeproject/VidoeDithering/sensor1_2021-05-24-15_54_55.h265'
    path2 = '/home/guod/VSCodeproject/VidoeDithering/outputsensor1SIFTVideo.h265'

    fcap1 = cv2.VideoCapture(path1)
    fcap2 = cv2.VideoCapture(path2)
    print("input width is:", str(fcap1.get(cv2.CAP_PROP_FRAME_WIDTH) ) )

    while fcap1.isOpened() and fcap2.isOpened():
        success1, frame1 = fcap1.read()
        success2, frame2 = fcap2.read()
        

        #If the last frame is reached, reset the capture and the frame_counter.
        if not success1 or not success2:
            fcap1.release()
            fcap2.release()
        else:
            #cv2.namedWindow('Original',cv2.WINDOW_NORMAL)
            cv2.imshow('Original',frame1)
            cv2.moveWindow('Original', 100, 270)

            #cv2.namedWindow('Output',cv2.WINDOW_NORMAL)
            cv2.imshow('Output',frame2)
            cv2.moveWindow('Output', 997,270)

        if (cv2.waitKey(20) & 0xff) == ord('q'):
            #wait 20 ms and press q to quit.
            break

    fcap1.release()
    fcap2.release()
    cv2.destroyAllWindows()
