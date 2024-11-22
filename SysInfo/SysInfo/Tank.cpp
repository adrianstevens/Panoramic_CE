#include "Tank.h"
#include "math.h"

CTank::CTank(void)
{
    ObjectType  = 0; // 0=cube, 1=tank, 2=saucer, 3=shell 
    HALFWIDTH = 200;
    HALFHEIGHT = 200;

    position_x = 0;      // This will be set by constructor.
    position_y = 10;      // which also defines object type
    position_z = 30;      // This class will be abstract!

    tdshape = NULL;

    m_iArraySize    = 0;

}

CTank::~CTank(void)
{
}

void CTank::Initialize(int object_type)
{

    // This method will define the points that make up 
    // the edges of a cube.
    // Notice how the array of points doesn't have be 
    // declared until it is
    // needed - and then, the size doesn't have to be 
    // fixed as with C or C++

    ObjectType = object_type;
    int i;

    if (object_type==1)
    {
        m_iArraySize = 24;

        // Cube
        tdshape = new tdpoint[24];
        i=0;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   2   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   -2   ;
    }

    if (object_type==2)
    {
        m_iArraySize = 34;

        // Tank
        // Cool tank shape!
        tdshape = new tdpoint[34];
        i=0;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   -2   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)1   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   1   ;
        tdshape[i++].z=   (float)-1   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   0   ;
        tdshape[i].x=   2   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   0   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)1.5   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   0   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   0   ;
        tdshape[i++].z=   (float)-1.5   ;
        tdshape[i].x=   1   ;
        tdshape[i].y=   -2   ;
        tdshape[i++].z=   0   ;
        tdshape[i].x=   0   ;
        tdshape[i].y=   (float)-1.5   ;
        tdshape[i++].z=   0   ;
        tdshape[i].x=   -3   ;
        tdshape[i].y=   (float)-1.5   ;
        tdshape[i++].z=   0   ;
    }

    if (object_type==3)
    {
        m_iArraySize = 4;

        // Flag
        // A little flag checkpoint shape.

        tdshape = new tdpoint[4];
        tdshape[0].x=0;
        tdshape[0].y=0;
        tdshape[0].z=0;
        tdshape[1].x=0;
        tdshape[1].y=-2;
        tdshape[1].z=0;
        tdshape[2].x=1;
        tdshape[2].y=(float)-1.5;
        tdshape[2].z=0;
        tdshape[3].x=0;
        tdshape[3].y=-1;
        tdshape[3].z=0;
    }
}

float CTank::GetX()
{
    // A method to return the object's X position
    return position_x;
}

float CTank::GetZ()
{
    // A method to return the object's Y position
    return position_z;
}

void CTank::SetPosition(float x, float y, float z)
{
    // A method to quickly set the position of the object
    position_x = x;
    position_y = y;
    position_z = z;
}

void CTank::Spin(float angle)
{
    // Spin the object around its own Y axis


    float zz,xx;

    // Save a little time by calculating these
    // Sin and Cosine values just the once..
    float sd=(float)sin(angle*3.141592654/180);
    float cd=(float)cos(angle*3.141592654/180);

    for (int j=0;j< m_iArraySize;j++)
    {
        xx=(tdshape[j].x*cd)-(tdshape[j].z*sd);
        zz=(tdshape[j].x*sd)+(tdshape[j].z*cd);    
        tdshape[j].z=zz;tdshape[j].x=xx;
    }
}

void CTank::Rotate(float angle)
{
    // Spin the object around the central point
    // which gives the impression the observer is
    // actually rotating

    float zz,xx;
    float sd=(float)sin(angle*3.141592654/180);
    float cd=(float)cos(angle*3.141592654/180);

    xx=(position_x*cd)-(position_z*sd);
    zz=(position_x*sd)+(position_z*cd);    
    position_z=zz;position_x=xx;
}

void CTank::Move(float distance)
{
    // Move the object
}


void CTank::Draw(HDC hdc, RECT& rcClient)
{
    // Draw the object!

    // These variables store the co-ords for line drawing
    float x1,y1,z1,x2,y2,z2;
    // These variables store the perspective co-ordinates
    float px1,py1,px2,py2;         
    // This differs from the "full" frameworks
    HPEN blackPen = (HPEN)CreatePen(0, 1, 0xFFFFFF);
    HPEN oldPen = (HPEN)SelectObject(hdc, blackPen);

    POINT pt[2];

    // Loop through all the points in the object and draw!
    for (int i=0;i< m_iArraySize;i+=2)
    {
        x1=tdshape[i].x+position_x;
        y1=tdshape[i].y+position_y;
        z1=tdshape[i].z+position_z;
        x2=tdshape[i+1].x+position_x;
        y2=tdshape[i+1].y+position_y;
        z2=tdshape[i+1].z+position_z;

        if ((z1>1) && (z2>1))
        {
            px1=200*(x1/z1);py1=200*(y1/z1);
            px2=200*(x2/z2);py2=200*(y2/z2);

            if (!((px1<-HALFWIDTH) && (px2<-HALFWIDTH)
                ||(px1>HALFWIDTH) && (px2>HALFWIDTH)))
            {   //grfx.DrawLine(blackPen,(int)(100+px1),(int)(100+py1),(int)(100+px2),(int)(100+py2));
                pt[0].x = (int)(100+px1);
                pt[0].y = (int)(100+py1);
                pt[1].x = (int)(100+px2);
                pt[1].y = (int)(100+py2);

                Polyline(hdc, pt, 2);
            }
        }
    }

    SelectObject(hdc, oldPen);
    DeleteObject(blackPen);
}

