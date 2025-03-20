/*********************************************************************/
/* PhotoLab.c: Assignment 6 for CENG231, Spring 2023                 */
/*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** global definitions ***/
#define WIDTH  500		/* image width */
#define HEIGHT 281		/* image height */
#define SLEN    80		/* maximum length of file names */

/*** function declarations ***/

/* Load an image from a file */
int LoadImage(const char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    FILE *File;
    char Type[SLEN];
    int  Width, Height, MaxValue;
    int  x, y;
    char ftype[] = ".ppm";
    char fname_ext[SLEN + sizeof(ftype)];

    strcpy(fname_ext, fname);
    strcat(fname_ext, ftype);

    File = fopen(fname_ext, "r");
    if (!File) {
        printf("\nCannot open file \"%s\" for reading!\n", fname);
        return 1;
    }
    fscanf(File, "%79s", Type);
    if (Type[0] != 'P' || Type[1] != '6' || Type[2] != 0) {
        printf("\nUnsupported file format!\n");
        return 2;
    }
    fscanf(File, "%d", &Width);
    if (Width != WIDTH) {
        printf("\nUnsupported image width %d!\n", Width);
        return 3;
    }
    fscanf(File, "%d", &Height);
    if (Height != HEIGHT) {
        printf("\nUnsupported image height %d!\n", Height);
        return 4;
    }
    fscanf(File, "%d", &MaxValue);
    if (MaxValue != 255) {
        printf("\nUnsupported image maximum value %d!\n", MaxValue);
        return 5;
    }
    if ('\n' != fgetc(File)) {
        printf("\nCarriage return expected!\n");
        return 6;
    }
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            R[x][y] = fgetc(File);
            G[x][y] = fgetc(File);
            B[x][y] = fgetc(File);
        }
    }
    if (ferror(File)) {
        printf("\nFile error while reading from file!\n");
        return 7;
    }
    printf("%s was read successfully!\n", fname_ext);
    fclose(File);
    return 0;
}

/* Save a processed image */
int SaveImage(const char fname[SLEN], unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    FILE *File;
    char ftype[] = ".ppm";
    char fname_ext[SLEN + sizeof(ftype)];
    char SysCmd[SLEN * 5];
    int  x, y;

    strcpy(fname_ext, fname);
    strcat(fname_ext, ftype);

    File = fopen(fname_ext, "w");
    if (!File) {
        printf("\nCannot open file \"%s\" for writing!\n", fname);
        return 1;
    }
    fprintf(File, "P6\n");
    fprintf(File, "%d %d\n", WIDTH, HEIGHT);
    fprintf(File, "255\n");

    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            fputc(R[x][y], File);
            fputc(G[x][y], File);
            fputc(B[x][y], File);
        }
    }

    if (ferror(File)) {
        printf("\nFile error while writing to file!\n");
        return 2;
    }
    fclose(File);
    printf("%s was saved successfully. \n", fname_ext);

    /*
     * rename file to image.ppm, convert it to ~/public_html/<fname>.jpg
     * and make it world readable
     */
    sprintf(SysCmd, "~ceng231/bin/pnmtojpeg.sh %s", fname_ext);
    if (system(SysCmd) != 0) {
        printf("\nError while converting to JPG:\nCommand \"%s\" failed!\n", SysCmd);
        return 3;
    }
    printf("%s.jpg was stored for viewing. \n", fname);

    return 0;
}

/* Age the image */
void Aging(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int x, y;

    for( y = 0; y < HEIGHT; y++ ) 
    {
        for( x = 0; x < WIDTH; x++ ) 
        {
	    B[x][y] = ( R[x][y]+G[x][y]+B[x][y] )/5;
            R[x][y] = (unsigned char) (B[x][y]*1.6);
            G[x][y] = (unsigned char) (B[x][y]*1.6);
	    }
    }
}

/* Change a color image to black & white */
void BlackNWhite(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int i, j, intensity;
    for (i = 0; i < WIDTH; i++) 
    {
        for (j = 0; j < HEIGHT; j++) 
        {
            /* Compute the average intensity */
            intensity = (R[i][j] + G[i][j] + B[i][j]) / 3;
            /* Assign the average intensity to all channels */
            R[i][j] = G[i][j] = B[i][j] = intensity;
        }
    }
}

/* Reverse the image color */
void Negative(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int i, j;
    for(i=0; i<WIDTH; i++) 
    {
        for(j=0; j<HEIGHT; j++) 
        {
            R[i][j] = 255 - R[i][j];
            G[i][j] = 255 - G[i][j];
            B[i][j] = 255 - B[i][j];
        }
    }
}

/* Flip an image vertically */
void VFlip(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int i, j;
    for (i = 0; i < WIDTH; i++) 
    {
        for (j = 0; j < HEIGHT / 2; j++) 
        {
            /* swap pixel (i,j) with pixel (i,HEIGHT-j-1) */
            unsigned char tmp = R[i][j];
            R[i][j] = R[i][HEIGHT-j-1];
            R[i][HEIGHT-j-1] = tmp;

            tmp = G[i][j];
            G[i][j] = G[i][HEIGHT-j-1];
            G[i][HEIGHT-j-1] = tmp;

            tmp = B[i][j];
            B[i][j] = B[i][HEIGHT-j-1];
            B[i][HEIGHT-j-1] = tmp;
        }
    }
}

/* Mirror an image horizontally */
void HMirror(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT]) 
{
    int i, j;
    unsigned char tempR, tempG, tempB;
    
    /* iterate through the columns of the right half of the image */
    for (i = WIDTH / 2; i < WIDTH; i++) 
    {
        /* iterate through the rows of the image */
        for (j = 0; j < HEIGHT; j++) 
        {
            /* swap the pixel at the right with the pixel at the corresponding position on the left */
            tempR = R[i][j];
            R[i][j] = R[WIDTH - i - 1][j];
            R[WIDTH - i - 1][j] = tempR;
            
            tempG = G[i][j];
            G[i][j] = G[WIDTH - i - 1][j];
            G[WIDTH - i - 1][j] = tempG;
            
            tempB = B[i][j];
            B[i][j] = B[WIDTH - i - 1][j];
            B[WIDTH - i - 1][j] = tempB;
        }
    }
    
    /* copy the right half of the image to the left half */
    for (i = 0; i < WIDTH / 2; i++) 
    {
        for (j = 0; j < HEIGHT; j++) 
        {
            R[i][j] = R[WIDTH - i - 1][j];
            G[i][j] = G[WIDTH - i - 1][j];
            B[i][j] = B[WIDTH - i - 1][j];
        }
    }
}




/* Sharpen an image */
void Sharpen(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int tmpR[WIDTH][HEIGHT], tmpG[WIDTH][HEIGHT], tmpB[WIDTH][HEIGHT];
    int i, j;

    /* Calculate sharpened pixel values */
    for (i = 1; i < HEIGHT - 1; i++) 
    {
        for (j = 1; j < WIDTH - 1; j++) 
        {
            tmpR[j][i] = -R[j-1][i-1] - R[j][i-1] - R[j+1][i-1]
                          - R[j-1][i]   + 9*R[j][i]   - R[j+1][i]
                          - R[j-1][i+1] - R[j][i+1] - R[j+1][i+1];
            tmpG[j][i] = -G[j-1][i-1] - G[j][i-1] - G[j+1][i-1]
                          - G[j-1][i]   + 9*G[j][i]   - G[j+1][i]
                          - G[j-1][i+1] - G[j][i+1] - G[j+1][i+1];
            tmpB[j][i] = -B[j-1][i-1] - B[j][i-1] - B[j+1][i-1]
                          - B[j-1][i]   + 9*B[j][i]   - B[j+1][i]
                          - B[j-1][i+1] - B[j][i+1] - B[j+1][i+1];
        }
    }

    /* Saturate and assign the new pixel values */
    for (i = 1; i < HEIGHT - 1; i++) 
    {
        for (j = 1; j < WIDTH - 1; j++) 
        {
            /* Saturate R channel */
            if (tmpR[j][i] > 255) 
            {
                R[j][i] = 255;
            } else if (tmpR[j][i] < 0) 
            {
                R[j][i] = 0;
            } else 
            {
                R[j][i] = (unsigned char) tmpR[j][i];
            }

            /* Saturate G channel */
            if (tmpG[j][i] > 255) 
            {
                G[j][i] = 255;
            } else if (tmpG[j][i] < 0) 
            {
                G[j][i] = 0;
            } else 
            {
                G[j][i] = (unsigned char) tmpG[j][i];
            }

            /* Saturate B channel */
            if (tmpB[j][i] > 255) 
            {
                B[j][i] = 255;
            } else if (tmpB[j][i] < 0) 
            {
                B[j][i] = 0;
            } else 
            {
                B[j][i] = (unsigned char) tmpB[j][i];
            }
        }
    }
}

/* exchange R and B color channels */
void ExRB(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    int i, j;
    for (i = 0; i < WIDTH; i++) 
    {
        for (j = 0; j < HEIGHT; j++) 
        {
            /* swap red and blue intensity values */
            unsigned char temp = R[i][j];
            R[i][j] = B[i][j];
            B[i][j] = temp;
        }
    }
}

/* [Bonus] Add salt-and-pepper noise to image */
void AddNoise(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], int percentage)
{
    int num_pixels = percentage * WIDTH * HEIGHT / 100;
    int i, j, k;
    for (k = 0; k < num_pixels; k++) 
    {
        i = rand() % WIDTH;
        j = rand() % HEIGHT;
        int color = rand() % 2;
        /* 0 = black, 1 = white */
        if (color == 0) 
        {
            R[i][j] = G[i][j] = B[i][j] = 0;
        } else 
        {
            R[i][j] = G[i][j] = B[i][j] = 255;
        }
    }
}

/* [Bonus] Add a border to the image */
void AddBorder(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT], int r, int g, int b, int bwidth)
{
    int i, j;
    /* Add top and bottom borders */
    for (i = 0; i < bwidth; i++) 
    {
        for (j = 0; j < WIDTH; j++) 
        {
            R[j][i] = r;
            G[j][i] = g;
            B[j][i] = b;
            R[j][HEIGHT-i-1] = r;
            G[j][HEIGHT-i-1] = g;
            B[j][HEIGHT-i-1] = b;
        }
    }
    /* Add left and right borders */
    for (i = bwidth; i < HEIGHT-bwidth; i++) 
    {
        for (j = 0; j < bwidth; j++) 
        {
            R[j][i] = r;
            G[j][i] = g;
            B[j][i] = b;
            R[WIDTH-j-1][i] = r;
            G[WIDTH-j-1][i] = g;
            B[WIDTH-j-1][i] = b;
        }
    }
}

/* Test all functions */
void AutoTest(unsigned char R[WIDTH][HEIGHT], unsigned char G[WIDTH][HEIGHT], unsigned char B[WIDTH][HEIGHT])
{
    LoadImage("wilson", R, G, B);
    Aging(R, G, B);
    SaveImage("aging", R, G, B);
    printf("Aging Tested!\n\n");

    LoadImage("wilson", R, G, B);
    BlackNWhite(R, G, B);
    SaveImage("bw", R, G, B);
    printf("Black & White Tested!\n\n");

    LoadImage("wilson", R, G, B);
    Negative(R, G, B);
    SaveImage("negative", R, G, B);
    printf("Negative Tested!\n\n");

    LoadImage("wilson", R, G, B);
    VFlip(R, G, B);
    SaveImage("vflip", R, G, B);
    printf("VFlip Tested!\n\n");

    LoadImage("wilson", R, G, B);
    HMirror(R, G, B);
    SaveImage("hmirror", R, G, B);
    printf("Horizontal Mirror Tested!\n\n");

    LoadImage("wilson", R, G, B);
    Sharpen(R, G, B);
    SaveImage("sharpen", R, G, B);
    printf("Sharpen Tested!\n\n");

    LoadImage("wilson", R, G, B);
    ExRB(R, G, B);
    SaveImage("exrb", R, G, B);
    printf("ExRB Tested!\n\n");

    LoadImage("wilson", R, G, B);
    AddNoise(R, G, B, 20);
    SaveImage("noise", R, G, B);
    printf("AddNoise tested!\n\n");

    LoadImage("wilson", R, G, B);
    AddBorder(R, G, B, 0, 0, 0, 20);
    SaveImage("border", R, G, B);
    printf("AddBorder tested!\n\n");

    /* all tests are here */
}

int main(void)
{
    /* Two dimensional arrays to hold the current image data, */
    /* one array for each color component.                    */
    unsigned char   R[WIDTH][HEIGHT];
    unsigned char   G[WIDTH][HEIGHT];
    unsigned char   B[WIDTH][HEIGHT];

    /* Please extend the AutoTest function for the other DIP operations */
    AutoTest(R, G, B);

    return 0;
}

/* EOF */
