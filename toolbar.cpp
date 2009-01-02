#include "solvespace.h"
#include "obj/icons.h"

BYTE SPACER[1];
static const struct {
    BYTE        *image;
    int          menu;
    char        *tip;
} Toolbar[] = {
    { Icon_line,            GraphicsWindow::MNU_LINE_SEGMENT,   "Sketch line segment"                               },
    { Icon_rectangle,       GraphicsWindow::MNU_RECTANGLE,      "Sketch rectangle"                                  },
    { Icon_circle,          GraphicsWindow::MNU_CIRCLE,         "Sketch circle"                                     },
    { Icon_arc,             GraphicsWindow::MNU_ARC,            "Sketch arc, or tangent arc at selected point"      },
    { Icon_bezier,          GraphicsWindow::MNU_CUBIC,          "Sketch cubic Bezier section"                       },
    { Icon_point,           GraphicsWindow::MNU_DATUM_POINT,    "Sketch datum point"                                },
    { Icon_construction,    GraphicsWindow::MNU_CONSTRUCTION,   "Toggle construction"                               },
    { Icon_trim,            GraphicsWindow::MNU_CONSTRUCTION,   "Split lines / curves where they intersect"         },
    { SPACER  },

    { Icon_length,          GraphicsWindow::MNU_DISTANCE_DIA,   "Constrain distance / diameter / length"            },
    { Icon_angle,           GraphicsWindow::MNU_ANGLE,          "Constrain angle"                                   },
    { Icon_horiz,           GraphicsWindow::MNU_HORIZONTAL,     "Constrain to be horizontal"                        },
    { Icon_vert,            GraphicsWindow::MNU_VERTICAL,       "Constrain to be vertical"                          },
    { Icon_parallel,        GraphicsWindow::MNU_PARALLEL,       "Constrain to be parallel or tangent"               },
    { Icon_perpendicular,   GraphicsWindow::MNU_PERPENDICULAR,  "Constrain to be perpendicular"                     },
    { Icon_pointonx,        GraphicsWindow::MNU_ON_ENTITY,      "Constrain point on line / curve / plane / face"    },
    { Icon_symmetric,       GraphicsWindow::MNU_SYMMETRIC,      "Constrain symmetric"                               },
    { Icon_ref,             GraphicsWindow::MNU_REFERENCE,      "Toggle reference dimension"                        },
    { SPACER  },

    { Icon_extrude,         GraphicsWindow::MNU_GROUP_EXTRUDE,  "New group extruding active sketch"                 },
    { Icon_sketch_in_plane, GraphicsWindow::MNU_GROUP_WRKPL,    "New group in new workplane (thru given entities)"  },
    { Icon_sketch_in_3d,    GraphicsWindow::MNU_GROUP_3D,       "New group in 3d"                                   },
    { Icon_assemble,        GraphicsWindow::MNU_GROUP_IMPORT,   "New group importing / assembling file"             },
    { SPACER  },

    { Icon_in3d,            GraphicsWindow::MNU_FREE_IN_3D,     "Sketch / constrain in 3d"                          },
    { Icon_ontoworkplane,   GraphicsWindow::MNU_SEL_WORKPLANE,  "Sketch / constrain in workplane"                   },
    { NULL  },
};

void GraphicsWindow::ToolbarDraw(void) {
    ToolbarDrawOrHitTest(0, 0, true, NULL);
}

bool GraphicsWindow::ToolbarMouseMoved(int x, int y) {
    x += ((int)width/2);
    y += ((int)height/2);
   
    int nh;
    bool withinToolbar = ToolbarDrawOrHitTest(x, y, false, &nh);
    if(!withinToolbar) nh = 0;

    if(nh != toolbarTooltipped) {
        // Don't let the tool tip move around if the mouse moves within the
        // same item.
        toolbarMouseX = x;
        toolbarMouseY = y;
        toolbarTooltipped = 0;
    }

    if(nh != toolbarHovered) {
        toolbarHovered = nh;
        SetTimerFor(1000);
        PaintGraphics();
    }
    // So if we moved off the toolbar, then toolbarHovered is now equal to
    // zero, so it doesn't matter if the tool tip timer expires. And if
    // we moved from one item to another, we reset the timer, so also okay.
    return withinToolbar;
}

bool GraphicsWindow::ToolbarMouseDown(int x, int y) {
    x += ((int)width/2);
    y += ((int)height/2);
   
    int nh;
    bool withinToolbar = ToolbarDrawOrHitTest(x, y, false, &nh);
    if(withinToolbar) {
        for(int i = 0; SS.GW.menu[i].level >= 0; i++) {
            if(nh == SS.GW.menu[i].id) {
                (SS.GW.menu[i].fn)((GraphicsWindow::MenuId)SS.GW.menu[i].id);
                break;
            }
        }
    }
    return withinToolbar;
}

bool GraphicsWindow::ToolbarDrawOrHitTest(int mx, int my,
                                          bool paint, int *menu)
{
    int i;
    int x = 17, y = (int)(height - 52);

    int fudge = 8;
    int h = 32*12 + 3*16 + fudge;
    int aleft = 0, aright = 66, atop = y+16+fudge/2, abot = y+16-h;

    bool withinToolbar =
        (mx >= aleft && mx <= aright && my <= atop && my >= abot);

    if(!paint && !withinToolbar) {
        // This gets called every MouseMove event, so return quickly.
        return false;
    }
    
    if(paint) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glTranslated(-1, -1, 0);
        glScaled(2.0/width, 2.0/height, 0);
        glDisable(GL_LIGHTING);

        double c = 30.0/255;
        glColor4d(c, c, c, 1.0);
        glBegin(GL_QUADS);
            glVertex2d(aleft,  atop);
            glVertex2d(aleft,  abot);
            glVertex2d(aright, abot);
            glVertex2d(aright, atop);
        glEnd();
    }

    struct {
        bool show;
        char *str;
    } toolTip = { false, NULL };

    bool leftpos = true;
    for(i = 0; Toolbar[i].image; i++) {
        if(Toolbar[i].image == SPACER) {
            if(!leftpos) {
                leftpos = true;
                y -= 32;
                x -= 32;
            }
            y -= 16;

            if(paint) {
                // Draw a separator bar in a slightly different color.
                int divw = 30, divh = 2;
                glColor4d(0.17, 0.17, 0.17, 1);
                x += 16;
                y += 24;
                glBegin(GL_QUADS);
                    glVertex2d(x+divw, y+divh);
                    glVertex2d(x+divw, y-divh);
                    glVertex2d(x-divw, y-divh);
                    glVertex2d(x-divw, y+divh);
                glEnd(); 
                x -= 16;
                y -= 24;
            }

            continue;
        }

        if(paint) {
            glRasterPos2i(x - 12, y - 12);
            glDrawPixels(24, 24, GL_RGB, GL_UNSIGNED_BYTE, Toolbar[i].image);

            if(toolbarHovered == Toolbar[i].menu) {
                // Highlight the hovered or pending item.
                glColor4d(1, 1, 0, 0.3);
                int boxhw = 15;
                glBegin(GL_QUADS);
                    glVertex2d(x+boxhw, y+boxhw);
                    glVertex2d(x+boxhw, y-boxhw);
                    glVertex2d(x-boxhw, y-boxhw);
                    glVertex2d(x-boxhw, y+boxhw);
                glEnd(); 
            }

            if(toolbarTooltipped == Toolbar[i].menu) {
                // Display the tool tip for this item; postpone till later
                // so that no one draws over us. Don't need position since
                // that's just wherever the mouse is.
                toolTip.show = true;
                toolTip.str = Toolbar[i].tip;
            }
        } else {
            int boxhw = 16;
            if(mx < (x+boxhw) && mx > (x - boxhw) &&
               my < (y+boxhw) && my > (y - boxhw))
            {
                if(menu) *menu = Toolbar[i].menu;
            }
        }

        if(leftpos) {
            x += 32;
            leftpos = false;
        } else {
            x -= 32;
            y -= 32;
            leftpos = true;
        }
    }

    if(paint) {
        // Do this last so that nothing can draw over it.
        if(toolTip.show) {
            char str[1024];
            if(strlen(toolTip.str) >= 200) oops();
            strcpy(str, toolTip.str);

            for(i = 0; SS.GW.menu[i].level >= 0; i++) {
                if(toolbarTooltipped == SS.GW.menu[i].id) {
                    int accel = SS.GW.menu[i].accel;
                    int ac = accel & 0xff;
                    if(isalnum(ac) || ac == '[') {
                        char *s = str+strlen(str);
                        if(accel & 0x100) {
                            sprintf(s, " (Shift+%c)", ac);
                        } else if((accel & ~0xff) == 0) {
                            sprintf(s, " (%c)", ac);
                        }
                    }
                    break;
                }
            }

            int tw, th;
            GetBitmapFontExtent(str, &tw, &th);
            tw += 10;
            th += 2;
            
            double ox = toolbarMouseX + 3, oy = toolbarMouseY + 3;
            glColor4d(1.0, 1.0, 0.6, 1.0);
            glBegin(GL_QUADS);
                glVertex2d(ox, oy);
                glVertex2d(ox+tw, oy);
                glVertex2d(ox+tw, oy+th);
                glVertex2d(ox, oy+th);
            glEnd();
            glColor4d(0.0, 0.0, 0.0, 1.0);
            glBegin(GL_LINE_LOOP);
                glVertex2d(ox, oy);
                glVertex2d(ox+tw, oy);
                glVertex2d(ox+tw, oy+th);
                glVertex2d(ox, oy+th);
            glEnd();

            glColor4d(0, 0, 0, 1);
            glPushMatrix();
                glRasterPos2d(ox+6, oy+6);
                DrawWithBitmapFont(str);
            glPopMatrix();
        }
        glxDepthRangeLockToFront(false);
    }

    return withinToolbar;
}

void GraphicsWindow::TimerCallback(void) {
    SS.GW.toolbarTooltipped = SS.GW.toolbarHovered;
    PaintGraphics();
}
