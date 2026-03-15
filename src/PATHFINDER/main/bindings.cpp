#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "pathfinder.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pathfinder, m) {
	// instructions
	py::enum_<Pathfinder::instruction_t>(m, "Instruction")
		.value("UP_LEFT", Pathfinder::instruction_t::up_left)
		.value("UP", Pathfinder::instruction_t::up)
		.value("UP_RIGHT", Pathfinder::instruction_t::up_right)
		.value("RIGHT", Pathfinder::instruction_t::right)
		.value("DOWN_RIGHT", Pathfinder::instruction_t::down_right)
		.value("DOWN", Pathfinder::instruction_t::down)
		.value("DOWN_LEFT", Pathfinder::instruction_t::down_left)
		.value("LEFT", Pathfinder::instruction_t::left)
		.value("SET_SPEED_0", Pathfinder::instruction_t::set_speed_0)
		.value("SET_SPEED_1", Pathfinder::instruction_t::set_speed_1)
		.value("SET_SPEED_2", Pathfinder::instruction_t::set_speed_2)
		.value("SET_SPEED_3", Pathfinder::instruction_t::set_speed_3)
		.value("MINE", Pathfinder::instruction_t::mine)
		.value("NO_INSTRUCTION", Pathfinder::instruction_t::no_instruction)
		.export_values();

	// route
	py::class_<Pathfinder::route_t>(m, "Route")
		.def(py::init<>())
		.def("__len__", &Pathfinder::route_t::size)
		.def("__getitem__", [](const Pathfinder::route_t& r, size_t i) {
			if (i >= r.size()) throw py::index_error();
			return r.at(i);
		})
		// make python list
		.def("to_list", [](const Pathfinder::route_t& r) {
			py::list lst;
			for (size_t i = 0; i < r.size(); ++i) {
				lst.append(r.at(i));
			}
			return lst;
		});

	// pathfinder
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