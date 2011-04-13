#include <ecto/module.hpp>
#include <ecto/ecto.hpp>

#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>
#include <ecto/python/std_map_indexing_suite.hpp>

#include <ecto/python/raw_constructor.hpp>
#include <ecto/python/repr.hpp>

namespace bp = boost::python;

namespace ecto
{
  namespace py
  {

    struct modwrap : module, bp::wrapper<module>
    {

      void Process()
      {
	if(bp::override process = this->get_override("Process"))
	  process();
	else
	  throw std::logic_error("Process is not overridden it seems");
      }

      void Config()
      {
	if (bp::override config = this->get_override("Config"))
	  config();
	else
	  throw std::logic_error("Config is not overridden it seems");
      }

      std::string name()
      {
        bp::reference_existing_object::apply<modwrap*>::type converter;
        PyObject* obj = converter(this);
        bp::object real_obj = bp::object(bp::handle<>(obj));
        bp::object n = real_obj.attr("__class__").attr("__name__");
        std::string nm = bp::extract<std::string>(n);
        return nm;
      }

      static std::string doc(modwrap* mod)
      {
        bp::reference_existing_object::apply<modwrap*>::type converter;
        PyObject* obj = converter(mod);
        bp::object real_obj = bp::object(bp::handle<>(obj));
        bp::object n = real_obj.attr("__class__").attr("__doc__");
        std::string nm = bp::extract<std::string>(n);
        return nm;
      }
    };

    void setTendril(tendrils& t, const std::string& name, const std::string& doc, bp::object o)
    {
      t.declare<bp::object>(name, doc, o);
    }

    bp::object getTendril(tendrils& t, const std::string& name)
    {
      return t[name].extract();
    }

    std::string strTendril(const tendrils& t)
    {
      std::string s = "tendrils:\n";
      for (tendrils::const_iterator iter = t.begin(), end = t.end();
	   iter != end;
	   ++iter)
	{
	  s += "    " + iter->first + " [" + iter->second.type_name() + "]\n";
	}
      return s;
    }

    bp::object tendril_get(const tendrils& ts, const std::string& name)
    {
      const tendril& t = ts.at(name);
      return t.extract();
    }

    void tendril_set(tendrils& ts, const std::string& name, bp::object obj)
    {
      ts.at(name).set(obj);
    }

    void wrapModule()
    {
      bp::class_<tendrils, boost::shared_ptr<tendrils>, boost::noncopyable>("tendrils")
	.def(bp::std_map_indexing_suite<tendrils, false>())
	.def("set", setTendril)
	.def("get", getTendril)
	.def("__str__", strTendril)
	.def("__getattr__", &tendril_get)
	.def("__setattr__", &tendril_set)
	;

      bp::class_<module, boost::shared_ptr<module>, boost::noncopyable>("module_cpp");

      bp::class_<modwrap, boost::shared_ptr<modwrap>, boost::noncopyable>("module_base"/*, bp::no_init*/)
	.def("connect", &module::connect)
	.def("Process", bp::pure_virtual(&module::Process))
	.def("Config", bp::pure_virtual(&module::Config))
	.def_readonly("inputs", &module::inputs)
	.def_readonly("outputs", &module::outputs)
	.def_readonly("params", &module::params)
	.def("Name", &module::name)
	.def("Doc", &modwrap::doc)
	;
    }

  }
}
