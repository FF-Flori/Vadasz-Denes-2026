#include "pathfinder.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

/* !!! nem akartam hozzányúlni, úgyhogy kikommenteztem, hogy tudjak buildelni

PYBIND11_MODULE(pathfinder, m) {
    py::class_<Pathfinder>(m, "Pathfinder")
        .def(py::init<>())
        .def("start", [](Pathfinder &self, const std::vector<std::vector<char>>& data) {
            return self.inputTransformation(data);
        });
}
*/