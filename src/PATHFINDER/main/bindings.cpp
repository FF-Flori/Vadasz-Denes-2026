#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pathfinder.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pathfinder, m) {
	py::class_<Pathfinder>(m, "Pathfinder")
		.def_static("create", &Pathfinder::create,
			py::arg("time_limit"),
			py::arg("map_path"),
			"Initializes singleton class with parameters")

		.def_static("get_instance", &Pathfinder::getInstance,
			py::return_value_policy::reference,
			"Returns instance")

		.def_static("destroy", &Pathfinder::destroy,
			"Destroys singleton class instance")

		.def("calculate", &Pathfinder::calculate,
			"Calculates an optimal path for the rover");
}