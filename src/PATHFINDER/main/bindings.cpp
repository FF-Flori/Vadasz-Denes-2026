#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pathfinder.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pathfinder, m) {
	py::class_<Pathfinder>(m, "pathfinder")
		.def(py::init<uint16_t, const std::string&>(),
				py::arg("time_limit"),
				py::arg("map_path"));
}
