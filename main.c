#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <OpenGL/OpenGL.h>
#include <OpenGL/glu.h>
#include <CoreGraphics/CoreGraphics.h>
#include <ApplicationServices/ApplicationServices.h>


int main (int argc, char **argv)
{
    CGLContextObj ctx;

    CGLPixelFormatAttribute attributes[] = {
        kCGLPFANoRecovery,
        kCGLPFAAccelerated,   // no software rendering
        kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
        kCGLPFADepthSize, (CGLPixelFormatAttribute)24,
        kCGLPFAAccelerated, (CGLPixelFormatAttribute)0,
        kCGLPFAOpenGLProfile, // core profile with the version stated below
        (CGLPixelFormatAttribute) kCGLOGLPVersion_3_2_Core,
        kCGLPFASampleBuffers,1,
        kCGLPFASamples,4,
        kCGLPFAMultisample
    };


    CGLPixelFormatObj pix;
    GLint num; // stores the number of possible pixel formats
    CGLChoosePixelFormat( attributes, &pix, &num );
    CGLCreateContext( pix, NULL, &ctx ); // second parameter can be another context for object sharing
    CGLDestroyPixelFormat( pix );
    CGLSetCurrentContext( ctx );

    CGLLockContext(ctx);


    GLuint  renderBuffer = 0;
    GLuint  depthBuffer = 0;
    int     img_width = 2000, img_height = 2000; // <-- pixel size of the rendered scene - hardcoded values for testing

    // Depth buffer to use for depth testing - optional if you're not using depth testing
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, img_width, img_height);

    // Render buffer to use for imaging
    glGenRenderbuffersEXT(1, &renderBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_RGBA8, img_width, img_height);

    GLuint  fbo = 0;
    glGenFramebuffersEXT(1, &fbo);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderBuffer);

    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);


	char kOutputFile[80];
	int i = 0;
    float x = 10.0;
    float y = 10.0;

    int samplesPerPixel = 4; // R, G, B and A
    int rowBytes = samplesPerPixel * img_width;
    char* bufferData = (char*)malloc(rowBytes * img_height);

	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
	CGBitmapInfo bitmapInfo = kCGImageAlphaNoneSkipFirst | kCGBitmapByteOrder32Little;	// XRGB Little Endian
	int bitsPerComponent = 8;

    CFIndex capacity = 1;

    glEnable(GL_MULTISAMPLE);

	for(i=0;i<10;i++){
		sprintf(kOutputFile,"window%04d.jpg",i);

		glEnable(GL_DEPTH_TEST);

		glClearColor(0.0, 0.0, 0.0, 1.0);

		glViewport(0, 0, img_width, img_height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(img_width / 2, -img_width / 2, img_height / 2, -img_height / 2, -1, 1);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClear(GL_COLOR_BUFFER_BIT);

		glClear(GL_DEPTH_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 400.0+y, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(200.0+x, -200.0-y, 0.0);
		glColor3f(1.0, 0.0, 1.0);
		glVertex3f(-200.0-x, -200.0-y, 0.0);
		glEnd();

        x += 50.0;
        y += 50.0;

		//Extract the resulting rendering as an image
		glReadPixels(0, 0, img_width, img_height, GL_BGRA, GL_UNSIGNED_BYTE, bufferData);

		// output the image as a file
        CGContextRef contextRef = CGBitmapContextCreate(bufferData,
				img_width, img_height, bitsPerComponent, rowBytes, colorSpace, bitmapInfo);
		CGImageRef imageRef = CGBitmapContextCreateImage(contextRef);

		CFURLRef fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
							CFStringCreateWithCString (NULL,kOutputFile,kCFStringEncodingUTF8), kCFURLPOSIXPathStyle, 0);

		CFIndex                 fileImageIndex = 1;
		CFMutableDictionaryRef  fileDict       = NULL;
		CFStringRef             fileUTType     = kUTTypeJPEG; //kUTTypePNG; //kUTTypeJPEG;

		// Create an image destination opaque reference for authoring an image file
		CGImageDestinationRef imageDest = CGImageDestinationCreateWithURL(fileURL,
																		  fileUTType,
																		  fileImageIndex,
																		  fileDict);

		CFMutableDictionaryRef imageProps = CFDictionaryCreateMutable(kCFAllocatorDefault,
												   capacity,
												   &kCFTypeDictionaryKeyCallBacks,
												   &kCFTypeDictionaryValueCallBacks);

		CGImageDestinationAddImage(imageDest, imageRef, imageProps);
		CGImageDestinationFinalize(imageDest);

		CFRelease(imageDest);
		CFRelease(fileURL);
		CFRelease(imageProps);

		CGImageRelease(imageRef);
        // end of save image stuff
	}

    free(bufferData);

	CGColorSpaceRelease( colorSpace );

    // unbind the FBO
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    CGLSetCurrentContext(NULL);
    CGLDestroyContext(ctx);

    return 0;
}
