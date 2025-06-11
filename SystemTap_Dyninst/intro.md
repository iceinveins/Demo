# <font color="3d8c95">SystemTap</font>
SystemTap is a tracing and probing tool that allows users to monitor the activities of the entire system without needing to instrument, recompile, install, and reboot. It is programmable with a custom scripting language, which gives it expressiveness (to trace, filter, and analyze) and reach (to look into the running kernel and applications).

## <font color="dc843f">Installing SystemTap</font>
```
scl enable devtoolset-10 'stap-prep'
scl enable devtoolset-10 'man stap'
scl enable devtoolset-10 'man staprun'
```

# <font color="3d8c95">Dyninst</font>
The Dyninst library provides an application programming interface (API) for instrumenting and working with user-space executables during their execution. It can be used to insert code into a running program, change certain subroutine calls, or even remove them from the program. It serves as a valuable debugging and performance-monitoring tool. The Dyninst API is also commonly used along with SystemTap to allow non-root users to instrument user-space executables.

## <font color="dc843f">Installing Dyninst</font>
```
yum install dyninst-devel
```
运行前 export LD_LIBRARY_PATH=/opt/rh/devtoolset-11/root/usr/lib64/dyninst/:$LD_LIBRARY_PATH