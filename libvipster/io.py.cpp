#include "pyvipster.h"
#include "io.h"

namespace Vipster::Py{
void PWInput(py::module&);
void LmpInput(py::module&);
void XYZ(py::module&);
void CPInput(py::module&);
void ORCA(py::module&);
void POSCAR(py::module&);

void IO(py::module& m){
    auto io = m.def_submodule("IO");

    // TODO: state handling
    m.def("readFile",[](std::string fn){
        auto data = readFile(fn);
        if(data.data.empty()){
            return py::make_tuple(data.mol, std::move(data.param), py::none());
        }else{
            py::list l{};
            for(auto& d: data.data){
                l.append(d.release());
            }
            return py::make_tuple(data.mol, std::move(data.param), l);
        }
    }, "filename"_a);
    m.def("readFile",[](std::string fn, std::string fmt){
        const auto plugins = IO::defaultPlugins();
        auto pos = std::find_if(plugins.begin(), plugins.end(), [&](const auto& plug){return plug->command == fmt;});
        if(pos == plugins.end()){
            throw IO::Error{"Invalid format given: "+fmt};
        }
        auto data = readFile(fn, *pos);
        if(data.data.empty()){
            return py::make_tuple(data.mol, std::move(data.param), py::none());
        }else{
            py::list l{};
            for(auto& d: data.data){
                l.append(d.release());
            }
            return py::make_tuple(data.mol, std::move(data.param), l);
        }
    }, "filename"_a, "format"_a);

    /*
     * TODO: provide wrapper
     *
     * fall back to default-preset/param
     * only index of state is relevant when not wrapping the GUI
     */
    m.def("writeFile", &writeFile, "filename"_a, "format"_a, "molecule"_a,
          "param"_a=nullptr, "config"_a=nullptr, "index"_a=-1ul);

    py::class_<IO::BaseParam>(io, "BaseParam");

    py::class_<IO::BasePreset>(io, "BasePreset");

    /*
     * Initialize plugins if needed
     */
    PWInput(io);
    LmpInput(io);
    XYZ(io);
    CPInput(io);
    ORCA(io);
    POSCAR(io);
}
}
