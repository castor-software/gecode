/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2006
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef GECODE_GIST_TREECANVAS_HH
#define GECODE_GIST_TREECANVAS_HH

#include "gecode/gist/gist.hh"
#include <QtGui>
// TODO nikopp: this include is just to get proper syntax highlighting in Eclipse
#include <qobject.h>

#include <gecode/kernel.hh>

namespace Gecode {
  
  class Space;
  
  namespace Gist {
  
  class VisualNode;

  class TreeCanvasImpl;
  
  class Searcher : public QThread {
    Q_OBJECT
  private:
    VisualNode* node;
    bool a;
    TreeCanvasImpl* t;
  public:
    void search(VisualNode* n, bool all, TreeCanvasImpl* ti);
    
  Q_SIGNALS:
    void update(void);
    
  protected:
    void run(void);
  };

  /// \brief Implementation of the TreeCanvas
  class TreeCanvasImpl : public QWidget {
    Q_OBJECT

    friend class Searcher;

  public:
    /// Constructor
    TreeCanvasImpl(Space* rootSpace, Better* b, QWidget* parent = NULL);
    /// Destructor
    ~TreeCanvasImpl(void);

    /// Set Inspector to \a i0
    void setInspector(Inspector* i);

  public Q_SLOTS:
    /// Set scale factor to \a scale0
    void scaleTree(int scale0);
    
    /// Explore complete subtree of selected node
    void searchAll(void);
    /// Find next solution below selected node
    void searchOne(void);
    /// Toggle hidden state of selected node
    void toggleHidden(void);
    /// Hide failed subtrees of selected node
    void hideFailed(void);
    /// Unhide all nodes below selected node
    void unhideAll(void);
    /// Export postscript of the tree
    void exportPostscript(void);
    /// Print the tree
    void print(void);
    /// Zoom the canvas so that the whole tree fits
    void zoomToFit(void);
    /// Center the view on the currently selected node
    void centerCurrentNode(void);
    /// Call the inspector for the currently selected node
    void inspectCurrentNode(void);
    
    /// Stop current search
    void stopSearch(void);
    
    /// Move selection to the parent of the selected node
    void navUp(void);
    /// Move selection to the first child of the selected node
    void navDown(void);
    /// Move selection to the left sibling of the selected node
    void navLeft(void);
    /// Move selection to the right sibling of the selected node
    void navRight(void);
    /// Recall selection of point in time \a i
    void markCurrentNode(int i);
    
    /// Set the current node to be the head of the path
    void setPath(void);
    /// Call the inspector for all nodes on the path from root to head of the path
    void inspectPath(void);

#ifdef GECODE_GIST_EXPERIMENTAL
    
   /// Add a new special node as child to the current node
    void addChild(void);
    /// Add a new fixpoint node as child to the current node if it is a special node
    void addFixpoint(void);
    /// Calls getVars(\a vm) on the space of the root node and \i is set to the most recent point in time
    void getRootVars(Gecode::Reflection::VarMap& vm, int& i);
    /// Investigate the current node
    void investigateCurrentNode(void);
    
#endif
    
  Q_SIGNALS:
    /// The scale factor has changed
    void scaleChanged(int);
    /// A new point in time was logged
    void newPointInTime(int);
    /// Context menu triggered
    void contextMenu(QContextMenuEvent*);
    
  protected:
    /// Mutex for synchronizing acccess to the tree
    QMutex mutex;
    /// Search engine thread
    Searcher searcher;
    /// Flag signalling the search to stop
    bool stopSearchFlag;
    /// The root node of the tree
    VisualNode* root;
    /// The currently selected node
    VisualNode* currentNode;
    /// The head of the currently selected path
    VisualNode* pathHead;
    /// The history of inspected nodes
    QVector<VisualNode*> nodeMap;
    /// The active inspector
    Inspector* inspect;
    
    /// Current scale factor
    double scale;
    /// Offset on the x axis so that the tree is centered
    int xtrans;

    /// Paint the tree
    void paintEvent(QPaintEvent* event);
    /// Handle mouse press event
    void mousePressEvent(QMouseEvent* event);
    /// Handle mouse double click event
    void mouseDoubleClickEvent(QMouseEvent* event);
    /// Handle context menu event
    void contextMenuEvent(QContextMenuEvent* event);
    /// Set the selected node to \a n
    void setCurrentNode(VisualNode* n);
    /// Log the current node as new point in time
    void saveCurrentNode(void);

  protected Q_SLOTS:
    /// Update display
    void update(void);
  };
  
  /// Tree canvas widget
  class TreeCanvas : public QWidget {
    Q_OBJECT
  private:
    /// The canvas implementation
    TreeCanvasImpl* canvas;
    QSlider* timeBar;
    /// Context menu
    QMenu* contextMenu;

  public:
    QAction* inspectCN;
    QAction* stopCN;
    QAction* navUp;
    QAction* navDown;
    QAction* navLeft;
    QAction* navRight;

    QAction* searchNext;
    QAction* searchAll;
    QAction* toggleHidden;
    QAction* hideFailed;
    QAction* unhideAll;
    QAction* zoomToFit;
    QAction* centerCN;
    QAction* exportPostscript;
    QAction* print;

    QAction* setPath;
    QAction* inspectPath;

#ifdef GECODE_GIST_EXPERIMENTAL
    QAction* addChild;
    QAction* addFixpoint;
    QAction* investigateCurrentNode;
#endif

  public:
    /// Constructor
    TreeCanvas(Space* root, Better* b = NULL, QWidget* parent = NULL);
    /// Destructor
    ~TreeCanvas(void);

    /// Set Inspector to \a i0
    void setInspector(Inspector* i0);

#ifdef GECODE_GIST_EXPERIMENTAL
    
  private Q_SLOTS:
    void on_canvas_newPointInTime(int);
#endif

  private Q_SLOTS:
    void on_canvas_contextMenu(QContextMenuEvent*);
    
  };
  
}}

#endif

// STATISTICS: gist-any
