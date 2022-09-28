#include <Python.h>
#include "graph.h"
#include "astar.h"

#define DEFAULT_SIZE 24

static graph_s_t *graph_;

static inline int bounds_check(graph_s_t *graph, int x) {
    if (x < 0 || x > graph->size - 1) {
        return 0;
    }
    return 1;
}

static PyObject *method_graph_reset(PyObject *self, PyObject *args) {
    graph_reset(graph_);
    Py_RETURN_NONE;
}

static PyObject *method_graph_set_size(PyObject *self, PyObject *args) {
    int size;
    if (!PyArg_ParseTuple(args, "i", &size)) {
        return NULL;
    }
    if (size <= 0) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    graph_set_size(graph_, size);
    Py_RETURN_NONE;
}

static PyObject *method_graph_get_size(PyObject *self, PyObject *args) {
    return PyLong_FromLong(graph_->size);
}

static PyObject *method_astar(PyObject *self, PyObject *args) {
    int x1, y1, x2, y2;
    if (!PyArg_ParseTuple(args, "iiii", &x1, &y1, &x2, &y2)) {
        return NULL;
    }
    if (!bounds_check(graph_, x1) || !bounds_check(graph_, x2) || !bounds_check(graph_, y1) ||
        !bounds_check(graph_, y2)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    point_s_t p1 = coord_to_index(graph_, x1, y1);
    point_s_t p2 = coord_to_index(graph_, x2, y2);
    if (astar(graph_, &graph_->arr[p1.x][p1.y], &graph_->arr[p2.x][p2.y], heuristic)) {
        vertex_s_t *vertex = &graph_->arr[p2.x][p2.y];
        PyObject *list = PyList_New(graph_->size * graph_->size + 1);
        int ctr = 2; // extra for start point and path length
        while (vertex->parent) {
            PyList_Append(list, Py_BuildValue("(ii)", vertex->coord.x, vertex->coord.y));
            vertex = vertex->parent;
            ctr++;
        }
        PyList_Append(list, Py_BuildValue("(ii)", graph_->arr[p1.x][p1.y].coord.x, graph_->arr[p1.x][p1.y].coord.y));
        PyList_Append(list, PyFloat_FromDouble(graph_->arr[p2.x][p2.y].g));
        PyList_Reverse(list);
        list = PyList_GetSlice(list, 0, ctr);
        return list;
    } else {
        PyErr_SetString(PyExc_Exception, "No path found!");
        return NULL;
    }
    Py_RETURN_NONE;
}

static PyObject *method_add_obstacle(PyObject *self, PyObject *args) {
    int x, y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y)) {
        return NULL;
    }
    if (!bounds_check(graph_, x) || !bounds_check(graph_, y)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    add_obstacle(graph_, x, y);
    Py_RETURN_NONE;
}

static PyObject *method_add_obstacles_rect(PyObject *self, PyObject *args) {
    int x1, y1, x2, y2;
    if (!PyArg_ParseTuple(args, "iiii", &x1, &y1, &x2, &y2)) {
        return NULL;
    }
    if (!bounds_check(graph_, x1) || !bounds_check(graph_, x2) || !bounds_check(graph_, y1) ||
        !bounds_check(graph_, y2)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    point_s_t p1 = coord_to_index(graph_, x1, y1);
    point_s_t p2 = coord_to_index(graph_, x2, y2);
    int iStart = p1.x < p2.x ? p1.x : p2.x;
    int iEnd = p1.x > p2.x ? p1.x : p2.x;
    int jStart = p1.y < p2.y ? p1.y : p2.y;
    int jEnd = p1.y > p2.y ? p1.y : p2.y;

    for (int i = iStart; i <= iEnd; ++i) {
        for (int j = jStart; j <= jEnd; ++j) {
            graph_->arr[i][j].deleted = 1;
        }
    }
    Py_RETURN_NONE;
}

static PyObject *method_remove_obstacle(PyObject *self, PyObject *args) {
    int x, y;
    if (!PyArg_ParseTuple(args, "ii", &x, &y)) {
        return NULL;
    }
    if (!bounds_check(graph_, x) || !bounds_check(graph_, y)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    remove_obstacle(graph_, x, y);
    Py_RETURN_NONE;
}

static PyObject *method_remove_obstacles_rect(PyObject *self, PyObject *args) {
    int x1, y1, x2, y2;
    if (!PyArg_ParseTuple(args, "iiii", &x1, &y1, &x2, &y2)) {
        return NULL;
    }
    if (!bounds_check(graph_, x1) || !bounds_check(graph_, x2) || !bounds_check(graph_, y1) ||
        !bounds_check(graph_, y2)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    point_s_t p1 = coord_to_index(graph_, x1, y1);
    point_s_t p2 = coord_to_index(graph_, x2, y2);
    int iStart = p1.x < p2.x ? p1.x : p2.x;
    int iEnd = p1.x > p2.x ? p1.x : p2.x;
    int jStart = p1.y < p2.y ? p1.y : p2.y;
    int jEnd = p1.y > p2.y ? p1.y : p2.y;

    for (int i = iStart; i <= iEnd; ++i) {
        for (int j = jStart; j <= jEnd; ++j) {
            graph_->arr[i][j].deleted = 0;
        }
    }
    Py_RETURN_NONE;
}

static PyObject *method_modify_weights(PyObject *self, PyObject *args) {
    int x1, y1, x2, y2, weight;
    if (!PyArg_ParseTuple(args, "iiiid", &x1, &y1, &x2, &y2, &weight)) {
        return NULL;
    }
    if (!bounds_check(graph_, x1) || !bounds_check(graph_, x2) || !bounds_check(graph_, y1) ||
        !bounds_check(graph_, y2)) {
        PyErr_SetString(PyExc_ValueError, "Arguments out of bounds!");
        return NULL;
    }
    if (weight <= 0) {
        PyErr_SetString(PyExc_ValueError, "Weight must be a positive value!");
        return NULL;
    }
    modify_weights(graph_, x1, y1, x2, y2, weight);
    Py_RETURN_NONE;
}

static PyMethodDef AStarMethods[] = {
        {"graph_reset",           method_graph_reset,           METH_VARARGS, "Resets all deleted vertices"},
        {"graph_set_size",        method_graph_set_size,        METH_VARARGS, "Changes graph size"},
        {"graph_get_size",        method_graph_get_size,        METH_VARARGS, "Gets the size of the graph"},
        {"run",                   method_astar,                 METH_VARARGS, "Runs A* from first coordinate to second"},
        {"add_obstacle",          method_add_obstacle,          METH_VARARGS, "Marks vertex as deleted"},
        {"add_obstacles_rect",    method_add_obstacles_rect,    METH_VARARGS, "Deletes vertices between coordinates"},
        {"remove_obstacle",       method_remove_obstacle,       METH_VARARGS, "Restores deleted vertex"},
        {"remove_obstacles_rect", method_remove_obstacles_rect, METH_VARARGS, "Restores vertices between coordinates"},
        {"modify_weight",         method_modify_weights,        METH_VARARGS, "Changes edge weight between two vertices"},
        {NULL, NULL, 0, NULL}
};

static void dtor(void *self) {
    free(graph_);
}

static struct PyModuleDef astarmodule = {
        .m_base = PyModuleDef_HEAD_INIT,
        .m_name = "astar",
        .m_doc ="AStar Algorithm in C",
        .m_size = -1,
        .m_methods = AStarMethods,
        .m_free = dtor
};

PyMODINIT_FUNC PyInit_astar() {
    PyObject *module = PyModule_Create(&astarmodule);
    graph_ = graph_new(DEFAULT_SIZE);
    return module;
}
