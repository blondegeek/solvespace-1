//-----------------------------------------------------------------------------
// Declarations relating to our user interface, in both the graphics and
// text browser window.
//
// Copyright 2008-2013 Jonathan Westhues.
//-----------------------------------------------------------------------------

#ifndef __UI_H
#define __UI_H

// This table describes the top-level menus in the graphics winodw.
enum class Command : uint32_t {
    NONE = 0,
    // File
    NEW = 100,
    OPEN,
    OPEN_RECENT,
    SAVE,
    SAVE_AS,
    EXPORT_PNG,
    EXPORT_MESH,
    EXPORT_SURFACES,
    EXPORT_VIEW,
    EXPORT_SECTION,
    EXPORT_WIREFRAME,
    IMPORT,
    EXIT,
    // View
    ZOOM_IN,
    ZOOM_OUT,
    ZOOM_TO_FIT,
    SHOW_GRID,
    PERSPECTIVE_PROJ,
    ONTO_WORKPLANE,
    NEAREST_ORTHO,
    NEAREST_ISO,
    CENTER_VIEW,
    SHOW_MENU_BAR,
    SHOW_TOOLBAR,
    SHOW_TEXT_WND,
    UNITS_INCHES,
    UNITS_MM,
    FULL_SCREEN,
    // Edit
    UNDO,
    REDO,
    CUT,
    COPY,
    PASTE,
    PASTE_TRANSFORM,
    DELETE,
    SELECT_CHAIN,
    SELECT_ALL,
    SNAP_TO_GRID,
    ROTATE_90,
    UNSELECT_ALL,
    REGEN_ALL,
    // Request
    SEL_WORKPLANE,
    FREE_IN_3D,
    DATUM_POINT,
    WORKPLANE,
    LINE_SEGMENT,
    CONSTR_SEGMENT,
    CIRCLE,
    ARC,
    RECTANGLE,
    CUBIC,
    TTF_TEXT,
    SPLIT_CURVES,
    TANGENT_ARC,
    CONSTRUCTION,
    // Group
    GROUP_3D,
    GROUP_WRKPL,
    GROUP_EXTRUDE,
    GROUP_LATHE,
    GROUP_ROT,
    GROUP_TRANS,
    GROUP_LINK,
    GROUP_RECENT,
    // Constrain
    DISTANCE_DIA,
    REF_DISTANCE,
    ANGLE,
    REF_ANGLE,
    OTHER_ANGLE,
    REFERENCE,
    EQUAL,
    RATIO,
    DIFFERENCE,
    ON_ENTITY,
    SYMMETRIC,
    AT_MIDPOINT,
    HORIZONTAL,
    VERTICAL,
    PARALLEL,
    PERPENDICULAR,
    ORIENTED_SAME,
    WHERE_DRAGGED,
    COMMENT,
    // Analyze
    VOLUME,
    AREA,
    INTERFERENCE,
    NAKED_EDGES,
    SHOW_DOF,
    TRACE_PT,
    STOP_TRACING,
    STEP_DIM,
    // Help
    WEBSITE,
    ABOUT,
    // Recent
    RECENT_OPEN = 0xf000,
    RECENT_LINK = 0xf100,
};

enum class ContextCommand : uint32_t {
    CANCELLED        = 0x000,
    SUBMENU          = 0x001,
    SEPARATOR        = 0x002,
    UNSELECT_ALL     = 0x100,
    UNSELECT_HOVERED = 0x101,
    CUT_SEL          = 0x102,
    COPY_SEL         = 0x103,
    PASTE            = 0x104,
    PASTE_XFRM       = 0x105,
    DELETE_SEL       = 0x106,
    SELECT_CHAIN     = 0x107,
    NEW_CUSTOM_STYLE = 0x110,
    NO_STYLE         = 0x111,
    GROUP_INFO       = 0x120,
    STYLE_INFO       = 0x121,
    REFERENCE_DIM    = 0x130,
    OTHER_ANGLE      = 0x131,
    DEL_COINCIDENT   = 0x132,
    SNAP_TO_GRID     = 0x140,
    REMOVE_SPLINE_PT = 0x141,
    ADD_SPLINE_PT    = 0x142,
    FIRST_STYLE      = 0x40000000
};

class TextWindow {
public:
    enum {
        MAX_COLS = 100,
        MIN_COLS = 45,
        MAX_ROWS = 2000
    };

    typedef struct {
        char      c;
        RgbaColor color;
    } Color;
    static const Color fgColors[];
    static const Color bgColors[];

    float bgColorTable[256*3];
    float fgColorTable[256*3];

    enum {
        CHAR_WIDTH     = 9,
        CHAR_HEIGHT    = 16,
        LINE_HEIGHT    = 20,
        LEFT_MARGIN    = 6,
    };

#define CHECK_FALSE "\xEE\x80\x80" // U+E000
#define CHECK_TRUE  "\xEE\x80\x81"
#define RADIO_FALSE "\xEE\x80\x82"
#define RADIO_TRUE  "\xEE\x80\x83"

    int scrollPos;      // The scrollbar position, in half-row units
    int halfRows;       // The height of our window, in half-row units

    uint32_t text[MAX_ROWS][MAX_COLS];
    typedef void LinkFunction(int link, uint32_t v);
    enum { NOT_A_LINK = 0 };
    struct {
        char            fg;
        char            bg;
        RgbaColor       bgRgb;
        int             link;
        uint32_t        data;
        LinkFunction   *f;
        LinkFunction   *h;
    }       meta[MAX_ROWS][MAX_COLS];
    int hoveredRow, hoveredCol;


    int top[MAX_ROWS]; // in half-line units, or -1 for unused
    int rows;

    // The row of icons at the top of the text window, to hide/show things
    typedef struct {
        bool       *var;
        const char *iconName;
        const char *tip;
        std::shared_ptr<Pixmap> icon;
    } HideShowIcon;
    static HideShowIcon hideShowIcons[];
    static bool SPACER;

    void Draw(Canvas *canvas);

    // These are called by the platform-specific code.
    void Paint();
    void MouseEvent(bool isClick, bool leftDown, double x, double y);
    void MouseScroll(double x, double y, int delta);
    void MouseLeave();
    void ScrollbarEvent(int newPos);

    enum DrawOrHitHow : uint32_t {
        PAINT = 0,
        HOVER = 1,
        CLICK = 2
    };
    void DrawOrHitTestIcons(UiCanvas *canvas, DrawOrHitHow how,
                            double mx, double my);
    void TimerCallback();
    Point2d oldMousePos;
    HideShowIcon *hoveredIcon, *tooltippedIcon;

    Vector HsvToRgb(Vector hsv);
    std::shared_ptr<Pixmap> HsvPattern2d(int w, int h);
    std::shared_ptr<Pixmap> HsvPattern1d(double hue, double sat, int w, int h);
    void ColorPickerDone();
    bool DrawOrHitTestColorPicker(UiCanvas *canvas, DrawOrHitHow how,
                                  bool leftDown, double x, double y);

    void Init();
    void MakeColorTable(const Color *in, float *out);
    void Printf(bool half, const char *fmt, ...);
    void ClearScreen();

    void Show();

    // State for the screen that we are showing in the text window.
    enum class Screen : uint32_t {
        LIST_OF_GROUPS      = 0,
        GROUP_INFO          = 1,
        GROUP_SOLVE_INFO    = 2,
        CONFIGURATION       = 3,
        STEP_DIMENSION      = 4,
        LIST_OF_STYLES      = 5,
        STYLE_INFO          = 6,
        PASTE_TRANSFORMED   = 7,
        EDIT_VIEW           = 8,
        TANGENT_ARC         = 9
    };
    typedef struct {
        Screen  screen;

        hGroup      group;
        hStyle      style;

        hConstraint constraint;
        bool        dimIsDistance;
        double      dimFinish;
        int         dimSteps;

        struct {
            int         times;
            Vector      trans;
            double      theta;
            Vector      origin;
            double      scale;
        }           paste;
    } ShownState;
    ShownState shown;

    enum class Edit : uint32_t {
        NOTHING               = 0,
        // For multiple groups
        TIMES_REPEATED        = 1,
        GROUP_NAME            = 2,
        GROUP_SCALE           = 3,
        GROUP_COLOR           = 4,
        GROUP_OPACITY         = 5,
        // For the configuraiton screen
        LIGHT_DIRECTION       = 100,
        LIGHT_INTENSITY       = 101,
        COLOR                 = 102,
        CHORD_TOLERANCE       = 103,
        MAX_SEGMENTS          = 104,
        CAMERA_TANGENT        = 105,
        GRID_SPACING          = 106,
        DIGITS_AFTER_DECIMAL  = 107,
        EXPORT_SCALE          = 108,
        EXPORT_OFFSET         = 109,
        CANVAS_SIZE           = 110,
        G_CODE_DEPTH          = 120,
        G_CODE_PASSES         = 121,
        G_CODE_FEED           = 122,
        G_CODE_PLUNGE_FEED    = 123,
        AUTOSAVE_INTERVAL     = 124,
        // For TTF text
        TTF_TEXT              = 300,
        // For the step dimension screen
        STEP_DIM_FINISH       = 400,
        STEP_DIM_STEPS        = 401,
        // For the styles stuff
        STYLE_WIDTH           = 500,
        STYLE_TEXT_HEIGHT     = 501,
        STYLE_TEXT_ANGLE      = 502,
        STYLE_COLOR           = 503,
        STYLE_FILL_COLOR      = 504,
        STYLE_NAME            = 505,
        BACKGROUND_COLOR      = 506,
        BACKGROUND_IMG_SCALE  = 507,
        STYLE_STIPPLE_PERIOD  = 508,
        // For paste transforming
        PASTE_TIMES_REPEATED  = 600,
        PASTE_ANGLE           = 601,
        PASTE_SCALE           = 602,
        // For view
        VIEW_SCALE            = 700,
        VIEW_ORIGIN           = 701,
        VIEW_PROJ_RIGHT       = 702,
        VIEW_PROJ_UP          = 703,
        // For tangent arc
        TANGENT_ARC_RADIUS    = 800
    };
    struct {
        bool        showAgain;
        Edit        meaning;
        int         i;
        hGroup      group;
        hRequest    request;
        hStyle      style;
    } edit;

    static void ReportHowGroupSolved(hGroup hg);

    struct {
        int     halfRow;
        int     col;

        struct {
            RgbaColor rgb;
            double    h, s, v;
            bool      show;
            bool      picker1dActive;
            bool      picker2dActive;
        }       colorPicker;
    } editControl;

    void HideEditControl();
    void ShowEditControl(int col, const std::string &str, int halfRow = -1);
    void ShowEditControlWithColorPicker(int col, RgbaColor rgb);

    void ClearSuper();

    void ShowHeader(bool withNav);
    // These are self-contained screens, that show some information about
    // the sketch.
    void ShowListOfGroups();
    void ShowGroupInfo();
    void ShowGroupSolveInfo();
    void ShowConfiguration();
    void ShowListOfStyles();
    void ShowStyleInfo();
    void ShowStepDimension();
    void ShowPasteTransformed();
    void ShowEditView();
    void ShowTangentArc();
    // Special screen, based on selection
    void DescribeSelection();

    void GoToScreen(Screen screen);

    // All of these are callbacks from the GUI code; first from when
    // we're describing an entity
    static void ScreenEditTtfText(int link, uint32_t v);
    static void ScreenSetTtfFont(int link, uint32_t v);
    static void ScreenUnselectAll(int link, uint32_t v);

    // when we're describing a constraint
    static void ScreenConstraintShowAsRadius(int link, uint32_t v);

    // and the rest from the stuff in textscreens.cpp
    static void ScreenSelectGroup(int link, uint32_t v);
    static void ScreenActivateGroup(int link, uint32_t v);
    static void ScreenToggleGroupShown(int link, uint32_t v);
    static void ScreenHowGroupSolved(int link, uint32_t v);
    static void ScreenShowGroupsSpecial(int link, uint32_t v);
    static void ScreenDeleteGroup(int link, uint32_t v);

    static void ScreenHoverConstraint(int link, uint32_t v);
    static void ScreenHoverRequest(int link, uint32_t v);
    static void ScreenSelectRequest(int link, uint32_t v);
    static void ScreenSelectConstraint(int link, uint32_t v);

    static void ScreenChangeGroupOption(int link, uint32_t v);
    static void ScreenColor(int link, uint32_t v);
    static void ScreenOpacity(int link, uint32_t v);

    static void ScreenShowListOfStyles(int link, uint32_t v);
    static void ScreenShowStyleInfo(int link, uint32_t v);
    static void ScreenDeleteStyle(int link, uint32_t v);
    static void ScreenChangeStylePatternType(int link, uint32_t v);
    static void ScreenChangeStyleYesNo(int link, uint32_t v);
    static void ScreenCreateCustomStyle(int link, uint32_t v);
    static void ScreenLoadFactoryDefaultStyles(int link, uint32_t v);
    static void ScreenAssignSelectionToStyle(int link, uint32_t v);
    static void ScreenBackgroundImage(int link, uint32_t v);

    static void ScreenShowConfiguration(int link, uint32_t v);
    static void ScreenShowEditView(int link, uint32_t v);
    static void ScreenGoToWebsite(int link, uint32_t v);

    static void ScreenChangeFixExportColors(int link, uint32_t v);
    static void ScreenChangeBackFaces(int link, uint32_t v);
    static void ScreenChangeCheckClosedContour(int link, uint32_t v);
    static void ScreenChangePwlCurves(int link, uint32_t v);
    static void ScreenChangeCanvasSizeAuto(int link, uint32_t v);
    static void ScreenChangeCanvasSize(int link, uint32_t v);
    static void ScreenChangeShadedTriangles(int link, uint32_t v);

    static void ScreenAllowRedundant(int link, uint32_t v);

    static void ScreenStepDimSteps(int link, uint32_t v);
    static void ScreenStepDimFinish(int link, uint32_t v);
    static void ScreenStepDimGo(int link, uint32_t v);

    static void ScreenChangeTangentArc(int link, uint32_t v);

    static void ScreenPasteTransformed(int link, uint32_t v);

    static void ScreenHome(int link, uint32_t v);

    // These ones do stuff with the edit control
    static void ScreenChangeExprA(int link, uint32_t v);
    static void ScreenChangeGroupName(int link, uint32_t v);
    static void ScreenChangeGroupScale(int link, uint32_t v);
    static void ScreenChangeLightDirection(int link, uint32_t v);
    static void ScreenChangeLightIntensity(int link, uint32_t v);
    static void ScreenChangeColor(int link, uint32_t v);
    static void ScreenChangeChordTolerance(int link, uint32_t v);
    static void ScreenChangeMaxSegments(int link, uint32_t v);
    static void ScreenChangeExportChordTolerance(int link, uint32_t v);
    static void ScreenChangeExportMaxSegments(int link, uint32_t v);
    static void ScreenChangeCameraTangent(int link, uint32_t v);
    static void ScreenChangeGridSpacing(int link, uint32_t v);
    static void ScreenChangeDigitsAfterDecimal(int link, uint32_t v);
    static void ScreenChangeExportScale(int link, uint32_t v);
    static void ScreenChangeExportOffset(int link, uint32_t v);
    static void ScreenChangeGCodeParameter(int link, uint32_t v);
    static void ScreenChangeAutosaveInterval(int link, uint32_t v);
    static void ScreenChangeStyleName(int link, uint32_t v);
    static void ScreenChangeStyleMetric(int link, uint32_t v);
    static void ScreenChangeStyleTextAngle(int link, uint32_t v);
    static void ScreenChangeStyleColor(int link, uint32_t v);
    static void ScreenChangeBackgroundColor(int link, uint32_t v);
    static void ScreenChangeBackgroundImageScale(int link, uint32_t v);
    static void ScreenChangePasteTransformed(int link, uint32_t v);
    static void ScreenChangeViewScale(int link, uint32_t v);
    static void ScreenChangeViewOrigin(int link, uint32_t v);
    static void ScreenChangeViewProjection(int link, uint32_t v);

    bool EditControlDoneForStyles(const char *s);
    bool EditControlDoneForConfiguration(const char *s);
    bool EditControlDoneForPaste(const char *s);
    bool EditControlDoneForView(const char *s);
    void EditControlDone(const char *s);
};

class GraphicsWindow {
public:
    void Init();

    typedef void MenuHandler(Command id);
    enum {
        ESCAPE_KEY = 27,
        DELETE_KEY = 127,
        FUNCTION_KEY_BASE = 0xf0
    };
    enum {
        SHIFT_MASK = 0x100,
        CTRL_MASK  = 0x200
    };
    enum class MenuKind : uint32_t {
        NORMAL = 0,
        CHECK,
        RADIO
    };
    typedef struct {
        int          level;          // 0 == on menu bar, 1 == one level down
        const char  *label;          // or NULL for a separator
        Command      id;             // unique ID
        int          accel;          // keyboard accelerator
        MenuKind     kind;
        MenuHandler  *fn;
    } MenuEntry;
    static const MenuEntry menu[];
    static void MenuView(Command id);
    static void MenuEdit(Command id);
    static void MenuRequest(Command id);
    void DeleteSelection();
    void CopySelection();
    void PasteClipboard(Vector trans, double theta, double scale);
    static void MenuClipboard(Command id);

    // The width and height (in pixels) of the window.
    double width, height;
    // These parameters define the map from 2d screen coordinates to the
    // coordinates of the 3d sketch points. We will use an axonometric
    // projection.
    Vector  offset;
    Vector  projRight;
    Vector  projUp;
    double  scale;
    struct {
        bool    mouseDown;
        Vector  offset;
        Vector  projRight;
        Vector  projUp;
        Point2d mouse;
        Point2d mouseOnButtonDown;
        Vector  marqueePoint;
        bool    startedMoving;
    }       orig;
    // We need to detect when the projection is changed to invalidate
    // caches for drawn items.
    struct {
        Vector  offset;
        Vector  projRight;
        Vector  projUp;
        double  scale;
    }       cached;

    // Most recent mouse position, updated every time the mouse moves.
    Point2d currentMousePosition;

    // When the user is dragging a point, don't solve multiple times without
    // allowing a paint in between. The extra solves are wasted if they're
    // not displayed.
    bool    havePainted;

    // Some state for the context menu.
    struct {
        bool        active;
    }       context;

    Camera GetCamera() const;
    Lighting GetLighting() const;

    void NormalizeProjectionVectors();
    Point2d ProjectPoint(Vector p);
    Vector ProjectPoint3(Vector p);
    Vector ProjectPoint4(Vector p, double *w);
    Vector UnProjectPoint(Point2d p);
    Vector UnProjectPoint3(Vector p);
    void AnimateOnto(Quaternion quatf, Vector offsetf);
    void AnimateOntoWorkplane();
    Vector VectorFromProjs(Vector rightUpForward);
    void HandlePointForZoomToFit(Vector p, Point2d *pmax, Point2d *pmin,
                                           double *wmin, bool usePerspective);
    void LoopOverPoints(const std::vector<Entity *> &entity, const std::vector<hEntity> &faces, Point2d *pmax, Point2d *pmin,
                        double *wmin, bool usePerspective, bool includeMesh);
    void ZoomToFit(bool includingInvisibles, bool useSelection = false);

    hGroup  activeGroup;
    void EnsureValidActives();
    bool LockedInWorkplane();
    void SetWorkplaneFreeIn3d();
    hEntity ActiveWorkplane();
    void ForceTextWindowShown();

    // Operations that must be completed by doing something with the mouse
    // are noted here.
    enum class Pending : uint32_t {
        NONE                        = 0,
        COMMAND                     = 1,
        DRAGGING_POINTS             = 2,
        DRAGGING_NEW_POINT          = 3,
        DRAGGING_NEW_LINE_POINT     = 4,
        DRAGGING_NEW_CUBIC_POINT    = 5,
        DRAGGING_NEW_ARC_POINT      = 6,
        DRAGGING_CONSTRAINT         = 7,
        DRAGGING_RADIUS             = 8,
        DRAGGING_NORMAL             = 9,
        DRAGGING_NEW_RADIUS         = 10,
        DRAGGING_MARQUEE            = 11,
    };

    struct {
        Pending              operation;
        Command              command;

        hRequest             request;
        hEntity              point;
        List<hEntity>        points;
        hEntity              circle;
        hEntity              normal;
        hConstraint          constraint;

        const char          *description;

        bool                 hasSuggestion;
        Constraint::Type     suggestion;
    } pending;
    void ClearPending();
    // The constraint that is being edited with the on-screen textbox.
    hConstraint constraintBeingEdited;

    bool SuggestLineConstraint(hRequest lineSegment, ConstraintBase::Type *type);

    Vector SnapToGrid(Vector p);
    bool ConstrainPointByHovered(hEntity pt);
    void DeleteTaggedRequests();
    hRequest AddRequest(Request::Type type, bool rememberForUndo);
    hRequest AddRequest(Request::Type type);

    class ParametricCurve {
    public:
        bool isLine; // else circle
        Vector p0, p1;
        Vector u, v;
        double r, theta0, theta1, dtheta;

        void MakeFromEntity(hEntity he, bool reverse);
        Vector PointAt(double t);
        Vector TangentAt(double t);
        double LengthForAuto();

        hRequest CreateRequestTrimmedTo(double t, bool extraConstraints,
            hEntity orig, hEntity arc, bool arcFinish);
        void ConstrainPointIfCoincident(hEntity hpt);
    };
    void MakeTangentArc();
    void SplitLinesOrCurves();
    hEntity SplitEntity(hEntity he, Vector pinter);
    hEntity SplitLine(hEntity he, Vector pinter);
    hEntity SplitCircle(hEntity he, Vector pinter);
    hEntity SplitCubic(hEntity he, Vector pinter);
    void ReplacePointInConstraints(hEntity oldpt, hEntity newpt);
    void RemoveConstraintsForPointBeingDeleted(hEntity hpt);
    void FixConstraintsForRequestBeingDeleted(hRequest hr);
    void FixConstraintsForPointBeingDeleted(hEntity hpt);

    // The current selection.
    class Selection {
    public:
        int         tag;

        hEntity     entity;
        hConstraint constraint;
        bool        emphasized;

        void Draw(bool isHovered, Canvas *canvas);

        void Clear();
        bool IsEmpty();
        bool Equals(Selection *b);
        bool HasEndpoints();
    };
    Selection hover;
    bool hoverWasSelectedOnMousedown;
    List<Selection> selection;
    void HitTestMakeSelection(Point2d mp);
    void ClearSelection();
    void ClearNonexistentSelectionItems();
    enum { MAX_SELECTED = 32 };
    struct {
        hEntity     point[MAX_SELECTED];
        hEntity     entity[MAX_SELECTED];
        hEntity     anyNormal[MAX_SELECTED];
        hEntity     vector[MAX_SELECTED];
        hEntity     face[MAX_SELECTED];
        hConstraint constraint[MAX_SELECTED];
        int         points;
        int         entities;
        int         workplanes;
        int         faces;
        int         lineSegments;
        int         circlesOrArcs;
        int         arcs;
        int         cubics;
        int         periodicCubics;
        int         anyNormals;
        int         vectors;
        int         constraints;
        int         stylables;
        int         constraintLabels;
        int         withEndpoints;
        int         n;
    } gs;
    void GroupSelection();
    bool IsSelected(Selection *s);
    bool IsSelected(hEntity he);
    void MakeSelected(hEntity he);
    void MakeSelected(hConstraint hc);
    void MakeSelected(Selection *s);
    void MakeUnselected(hEntity he, bool coincidentPointTrick);
    void MakeUnselected(Selection *s, bool coincidentPointTrick);
    void SelectByMarquee();
    void ClearSuper();

    void ContextMenuListStyles();
    int64_t contextMenuCancelTime;

    // The toolbar, in toolbar.cpp
    bool ToolbarDrawOrHitTest(int x, int y, UiCanvas *canvas, Command *menuHit);
    void ToolbarDraw(UiCanvas *canvas);
    bool ToolbarMouseMoved(int x, int y);
    bool ToolbarMouseDown(int x, int y);
    static void TimerCallback();
    Command toolbarHovered;
    Command toolbarTooltipped;
    int toolbarMouseX, toolbarMouseY;

    // This sets what gets displayed.
    bool    showWorkplanes;
    bool    showNormals;
    bool    showPoints;
    bool    showConstraints;
    bool    showTextWindow;
    bool    showShaded;
    bool    showEdges;
    bool    showOutlines;
    bool    showFaces;
    bool    showMesh;
    bool    showHdnLines;
    void ToggleBool(bool *v);

    bool    showSnapGrid;
    void DrawSnapGrid(Canvas *canvas);

    void AddPointToDraggedList(hEntity hp);
    void StartDraggingByEntity(hEntity he);
    void StartDraggingBySelection();
    void UpdateDraggedNum(Vector *pos, double mx, double my);
    void UpdateDraggedPoint(hEntity hp, double mx, double my);

    void DrawPersistent(Canvas *canvas);
    void Draw(Canvas *canvas);

    // These are called by the platform-specific code.
    void Paint();
    void MouseMoved(double x, double y, bool leftDown, bool middleDown,
                                bool rightDown, bool shiftDown, bool ctrlDown);
    void MouseLeftDown(double x, double y);
    void MouseLeftUp(double x, double y);
    void MouseLeftDoubleClick(double x, double y);
    void MouseMiddleOrRightDown(double x, double y);
    void MouseRightUp(double x, double y);
    void MouseScroll(double x, double y, int delta);
    void MouseLeave();
    bool KeyDown(int c);
    void EditControlDone(const char *s);

    int64_t lastSpaceNavigatorTime;
    hGroup lastSpaceNavigatorGroup;
    void SpaceNavigatorMoved(double tx, double ty, double tz,
                             double rx, double ry, double rz, bool shiftDown);
    void SpaceNavigatorButtonUp();
};


#endif
