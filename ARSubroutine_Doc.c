
/*
 *
 *————————————–————————————–————————————–————————————–———
 *   ARToolKit Step             Functions               |
 *———————————————————————————————————–—-————————————–————
 * 1. Initialize the video.  |  setupCamera &           |
 * grabbing from the camera  |  setupMarker             |
 * and load the marker(s)    |                          |
 *                           |                          |
 * 2. Grab a video input     |  arVideoGetImage         |
 * frame                     |  (called in mainLoop)    |
 *                           |                          |
 * 3. Detect the markers     |  arDetectMarker          |
 *                           |  (called in mainLoop)    |
 *                           |                          |
 * 4. Calculate camera       |  arGetTransMat           |
 * transformation            |  (called in mainLoop)    |
 *                           |                          |
 * 5. Draw the virtual       |  Display                 |
 * objects                   |                          |
 *                           |                          |
 * 6. Close the application  |  Quit                    |
 * down                      |                          |
 *————————————–————————————–————————————–————————————–———
 *333365124sg
 *
 */

/*******************
 *In this block, we define the pathname of the Calibrating Your Camera 
 *the application will use,
 *the Configuring Video Capture in ARToolKit, 
 *and the name of the marker pattern file the application will load and try to recognize.
 ******************
*/
    char cparam_name[] = "Data/camera_para.dat";
    char vconf[] = "";
    //ADDING TARGET PATTERN
    char patt_name[]  = "Data/hiro.patt";
    //can add more pattern by using 2 dimensional array
    //which contains all patterns




/******************
 *Video Setup
 ******************
 */
    if (!setupCamera(cparam_name, vconf, &gCparamLT, &gARHandle, &gAR3DHandle)) {
        ARLOGe("main(): Unable to set up AR camera.\n");
        exit(-1);
    }

/*setupCamera loads a file containing calibration parameters for a camera, 
 *opens a connection to the camera, 
 *sets some defaults (the binarization threshold in this case) 
 *and starts grabbing frames. 
 *
 *It records its settings into 3 variables which are passed in as parameters.
 *In our case, we will store these parameters in global variables.
 */




/******************
 *Library setup.
 ******************
 */
    // Set up GL context(s) for OpenGL to draw into.
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    if (!prefWindowed) {
        if (prefRefresh) sprintf(glutGamemode, "%ix%i:%i@%i", prefWidth, prefHeight, prefDepth, prefRefresh);
        else sprintf(glutGamemode, "%ix%i:%i", prefWidth, prefHeight, prefDepth);
        glutGameModeString(glutGamemode);
        glutEnterGameMode();
    } else {
        glutInitWindowSize(prefWidth, prefHeight);
        glutCreateWindow(argv[0]);
    }
/*opens up a window for us to draw into. 
 *This code uses GLUT to open the window. 
 *Later, we will install some event handlers for the window,
 *to handle redrawing, resizing etc.
 *The code uses the value of a variable
 * “prefWindowed” 
 *to decide whether to open a window, or whether to use fullscreen mode. 
 *Other variables:
 *prefWidth, prefHeight, prefDepth and prefRefresh are read to,
 *decide how many pixels wide and tall, what colour bit depth to use, and whether to change the refresh rate of the display. 
 *In simpleLite, these preferences are simply held in static variables defined near the top of main.c, 
 *but you could easily extend the example to load and save these preferences from and to a file, and allow the user to change them.
 */



/******************
 *Library setup.
 ******************
 */

   // Setup ARgsub_lite library for current OpenGL context.
   if ((gArglSettings = arglSetupForCurrentContext(gARHandle)) == NULL) {
       fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
       Quit();
   }
   debugReportMode(gARHandle, gArglSettings);
   glEnable(GL_DEPTH_TEST);
   arUtilTimerReset();


/* 
 *now that we have a window from GLUT, we initialize the OpenGL part of our application.
 *In this case, we are using the ARgsublite library to manage the interaction between the ARToolKit video capture and tracking, and OpenGL. 
 *At this point we also print out some debugging information in the user's console (or command line window),
 *showing the startup parameters for the application. 
 *As well, we reset ARToolKit's timer (which can be used to keep track of frame rates).
*/



/******************
 *Camera setup.
 ******************
 */

/*
 *The third major part of ARToolKit initialization is to load one or more markers which the camera should track.
 *Information about the markers has previously been recorded into marker pattern files
 * using the mkpatt utility (called “marker training”), 
 * so now we can load these files. 
 * In simpleLite, we will just use one marker, the default Hiro marker. 
 * The task of loading this marker and telling ARToolKit to track it,
 * is performed by the function called setupMarker()
 */


/* Before entering a real-time tracking and drawing state, we need to initialize the ARToolKit application parameters.
 * setupCamera and setupMarker
 * The key parameters for an ARToolKit application are:
 *       1:the patterns that will be used for the pattern template matching and the virtual objects
 *         these patterns correspond to(augmented objects).
 *          Target vs Augmented_obj
 *       2:the camera characteristics of the video camera being used.
 *
 * setupCamera begins by opening a connection to the video camera from which images for tracking will be acquired, 
 * using arVideoOpen(). 
 * The parameter vconf, passed to arVideoOpen is a string that can be used 
 * to request some video configuration other than the default. 
 * The contents of the vconf string are dependent on the video library being used. 
 * At this point we also find out from the video camera library how big the images it will supply will be, 
 * and what pixel format will be used
*/

 static int setupCamera(const char *cparam_name, char *vconf, int threshold, ARParam *cparam, ARHandle **arhandle, AR3DHandle **ar3dhandle)
    {
        ARParam         wparam;
        int             xsize, ysize;
        int             pixFormat;

        // Open the video path.
        if (arVideoOpen(vconf) < 0) {
            fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
            return (FALSE);
        }

        // Find the size of the window.
        if (arVideoGetSize(&xsize, &ysize) < 0) return (FALSE);
        fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);

        // Get the format in which the camera is returning pixels.
        pixFormat = arVideoGetPixelFormat();
        if (pixFormat < 0 ) {
            fprintf(stderr, "setupCamera(): Camera is using unsupported pixel format.\n");
            return (FALSE);
        }
        //now debug mode, the labelling threshold, and the structure used to hold positions of detected patterns have been initialized.


/*******************
 *Finally, we start the video library capturing frames,
 *since we will soon be ready to process them
 *******************/

    if (arSetDebugMode(*arhandle, AR_DEBUG_DISABLE) < 0) {
            fprintf(stderr, "setupCamera(): Error: arSetDebugMode.\n");
            return (FALSE);
        }
        if (arSetLabelingThresh(*arhandle, threshold) < 0) {
            fprintf(stderr, "setupCamera(): Error: arSetLabelingThresh.\n");
            return (FALSE);
        }
        if ((*ar3dhandle = ar3DCreateHandle(cparam)) == NULL) {
            fprintf(stderr, "setupCamera(): Error: ar3DCreateHandle.\n");
            return (FALSE);
        }

        if (arVideoCapStart() != 0) {
            fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
            return (FALSE);
        }

        return (TRUE);

    }




/******************
 *Marker setup.
 ******************
 */
/*
 *The second major part of ARToolKit setup is to load pattern files for each of the patterns we wish to detect.
 *In simpleLite, we will only track one pattern,
 *the basic “Hiro” pattern. 
 *setupMarker creates a list of patterns for ARToolKit to track, and loads the Hiro pattern into it.
 *Loading multiple patterns can be seen in the simpleVRML example, 
 *and is covered in a later chapter of the documentation.
*/
    static int setupMarker(const char *patt_name, int *patt_id, ARHandle *arhandle, ARPattHandle **pattHandle)
    {

    if ((*pattHandle = arPattCreateHandle()) == NULL) {
        fprintf(stderr, "setupCamera(): Error: arPattCreateHandle.\n");
        return (FALSE);
    }

    // Loading only 1 pattern in this example.
    if ((*patt_id = arPattLoad(*pattHandle, patt_name)) < 0) {
        fprintf(stderr, "setupMarker(): pattern load error !!\n");
        arPattDeleteHandle(*pattHandle);
        return (FALSE);
    }

    arPattAttach(arhandle, *pattHandle);

    return (TRUE);

    }



/*******************************************************************************************************
 *                                              MAIN LOOP setup.
 *******************************************************************************************************
 */
/*This is the routine where the bulk of the ARToolKit function calls are made 
 *and it contains code corresponding to steps 2 through 5 of the required application steps.
 */

/*First a new video frame is requested using the function arVideoGetImage. 
 *If the function returns non-NULL, a new frame has been captured,
 *and the return value points to the buffer containing the frame's pixel data, 
 *so we save it in a global variable.
 */
    // Grab a video frame.
    if ((image = arVideoGetImage()) != NULL) {
        gARTImage = image;  // Save the fetched image.
        gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
    }


/*Every time a new frame has been acquired, 
 *it needs to be searched for markers. 
 *This is accomplished by a call to the function arDetectMarker(),
 *passing in the pointer to the new frame, and an ARHandle. 
 *The ARHandle holds the ARToolKit marker detection settings and also stores the results of the marker detection.
*/

    // Detect the markers in the video frame.
    if (arDetectMarker(gARHandle, gARTImage) < 0) {
        exit(-1);
        }


/*The results of the marker detection process can now be examined in detail, 
 *to check whether they match the IDs of the marker(s) we loaded earlier.
 *Of course, in simpleLite, we only need to check for one marker, the Hiro marker. 
 *We use a value known as the marker confidence to make sure that we have got the Hiro marker 
 *and not a marker with a different pattern.
*/

        // Check through the marker_info array for highest confidence
        // visible marker matching our preferred pattern.
        k = -1;
        for (j = 0; j < gARHandle->marker_num; j++) {
            if (gARHandle->markerInfo[j].id == gPatt_id) {
                if (k == -1) k = j; // First marker detected.
                else if (gARHandle->markerInfo[j].cf > gARHandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
            }
        }


/*At the end of this loop, if k has been modified, 
 *then we have found the marker containing the Hiro pattern, 
 *so the last task we ask ARToolKit to perform on the marker is to retrieve its position and orientation 
 *(its “pose”) relative to the camera. 
 *The pose is stored in an AR3DHandle structure, which we conveniently prepared earlier (in setupCamera)!
 *If the marker is not found, we also note that fact, because if no markers are found, we should not try to draw any 3D objects in the frame.
 */

       if (k != -1) {
            // Get the transformation between the marker and the real camera into gPatt_trans.
            err = arGetTransMatSquare(gAR3DHandle, &(gARHandle->markerInfo[k]), gPatt_width, gPatt_trans);
            gPatt_found = TRUE;
        } else {
            gPatt_found = FALSE;
        }


//Finally, since we have a new video frame, we request that the operating system call our Display function:


    glutPostRedisplay();


//Display

/*We can conceive of our program running two loops in parallel;
 *one (in mainLoop()) grabs images from the camera and looks for markers in them. 
 *The other loop displays images and 3D objects over the top of detected marker positions,
 * or other AR-related content we might want to draw.
 *
 *These two loops run separately, 
 *because the operating system separates drawing from other regular tasks, to work more efficiently.
 *In simpleLite, the drawing all happens in the function named Display().
 *(This gets called by the operating system via GLUT).
 *In the display function, we do several steps: 
 *1      Clear the screen and draw the most recent frame from the camera as a video background.
 *2      Set up the OpenGL camera projection to match the calibrated ARToolKit camera parameters.
 *3      Check whether we have any active markers, and if so, position the OpenGL camera view for each one to place the coordinate system origin onto the marker.
 *4      Draw objects on top of any active markers (using the OpenGL camera).
 */


   /* Step 1: Clear the screen and draw the most recent frame from the camera as a video background:
    *The video image is then displayed on screen. 
    *This can either be an unwarped image, or an image warped to correct for camera distortions. 
    *Unwarping the camera's distorted image helps the virtual 3D objects appear in the correct place on the video frame.
    */
   // Select correct buffer for this context.
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffers for new frame.

    arglDispImage(gARTImage, &gARTCparam, 1.0, gArglSettings);  // zoom = 1.0.
    gARTImage = NULL;



   /*Step 2: Set up the OpenGL camera projection to match the calibrated ARToolKit camera parameters.
    *The call to arglCameraFrustumRH converts the camera parameters stored in gARTCparam into
    *an OpenGL projection matrix p, which is then loaded directly, setting the OpenGL camera projection. 
    *With this, the field-of-view, etc. of the real camera will be exactly matched in the scene.
    */

    // Projection transformation.
    arglCameraFrustumRH(&gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(p);
    glMatrixMode(GL_MODELVIEW);

    // Viewing transformation.
    glLoadIdentity();



    /*Step 3: Check whether we have any active markers, and if so, position the OpenGL camera view for each one to place the coordinate system origin onto the marker.
     *arglCameraViewRH converts the marker transformation (saved in mainLoop) into an OpenGL modelview matrix.
     *These sixteen values are the position and orientation values of the real camera, 
     *so using them to set the position of the virtual camera causes 
     *any graphical objects to be drawn to appear exactly aligned with the corresponding physical marker.
     *The virtual camera position is set using the OpenGL function glLoadMatrixd(m).
     */

    if (gPatt_found) {

        // Calculate the camera position relative to the marker.
        // Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
        arglCameraViewRH(gPatt_trans, m, VIEW_SCALEFACTOR);
        glLoadMatrixd(m);
    }


    /*Step 4: Draw objects on top of any active markers (using the OpenGL camera).
     *Finally, The last part of the code is the rendering of the 3D object, 
     *in this example the OpenGL color cube.
     *This function is simply an example (look at the code for drawCube if you are curious) 
     *and can be replaced with any drawing code.
     *Simply, at the time the draw function is called, 
     *the OpenGL coordinate system origin is exactly in the middle of the marker, 
     *with the marker lying in the x-y plane (x to the right, y upwards) 
     *and with the z axis pointing towards the viewer.
     *If you are drawing directly onto a marker, remember not to draw in the -z part of the OpenGL coordinate system, 
     *or else your drawing will look odd, as it will be drawn “behind” the marker.
     */



//cleanup
    //The cleanup function is called to stop ARToolKit and release resources used by it, in a clean manner:

    static void cleanup(void) {
        arglCleanup(gArglSettings);
        arPattDetach(gARHandle);
        arPattDeleteHandle(gARPattHandle);
        arVideoCapStop();
        ar3DDeleteHandle(gAR3DHandle);
        arDeleteHandle(gARHandle);
        arVideoClose();
    }

/*Cleanup steps are generally performed in reverse order to setup steps. 
 *NB: your application may have to perform other cleanup steps,
 *but these are the ones required by ARToolKit.
  Cleanup step                                                                                !Functions to be called.
 ------------------------------------------------------------------------------------------- --------------------------------------------------------------------
 remove ARToolKit's connections to OpenGL                                                    arglCleanup
 release resources used in the marker tracking                                               arPattDetach, arPattDeleteHandle, ar3DDeleteHandle, arDeleteHandle
 stop the video capture and close down the video path to free it up for other applications   arVideoCapStop, arVideoClose

 */
