FAQ {#FAQ}
===

Contents {#faqcontents}
=======

* [General](@ref general)
* [SE(3)](@ref se3)
* [Common Errors](@ref errors)
* [Performance](@ref performance)
* [Comparison with Other Packages](@ref comparison)


General {#general}
====

* Q: What is Amino?

  - A: Amino is a lightweight library for robot modeling, control, and
    visualization.

* Q: What platforms are supported?

  - A: Amino is developed on Debian and Ubuntu Linux.  It should run
    on other Linux distributions and POSIX-y systems with minimal
    effort.  Currently, non-POSIX platforms are not targeted.

SE(3) {#se3}
====

* Q: Why dual quaternions?

  - A: Dual quaternions are more compact and computational easier to
    normalize and filter than matrices.

* [Contents](@ref faqcontents)

Scene Graphs {#scenegraphs}
============

* Q: How can I load a URDF file?

  - A: Use the scene graph compiler, `aarxc`, to generate code from the
    URDF file.  Note that URDF support has additional dependencies;
    see `./INSTALL` for details.

* Q: How can I reload a modified scene graph file without restarting my
  program?

  - A: Compile the scene graph to a shared library and load the library
    with dlopen().  To reload the scene graph, recompile the scene
    graph and dlopen() the library again.

* [Contents](@ref faqcontents)

Common Errors {#errors}
=============

* Q: `./configure` fails with when checking for cffi-grovel.

    - A: Older versions of SBCL (around 1.2.4) have issues with current
      versions of CFFI.  Please try installing a recent SBCL (>1.3.4).

* Q: I get error messages about missing .obj files or Blender being
  unable to convert a .dae to Wavefront OBJ.

  - A: We use Blender to convert various mesh formats to Wavefront OBJ,
    then import the OBJ file.  The Blender binaries in the Debian and
    Ubuntu repositories (as of Jessie and Trusty) are not built with
    COLLADA (.dae) support.  You can download the prebuilt binaries
    from http://www.blender.org/ which do support COLLADA.

* Q: When I try to compile a URDF file, I receive the error "aarx.core:
  not found".

  - A: URDF support in amino is only built if the necessary dependencies
    are installed.  Please ensure that you have SBCL, Quicklisp, and
    Sycamore installed and rebuild amino if necessary.  See
    `./INSTALL` for details.

* Q: When building aarx.core, I get an enormous stack trace, starting
  with:
  `Unable to load any of the alternatives:
     ("libamino_planning.so" (:DEFAULT "libamino_planning"))`.

  - A: This means that SBCL is unable to load the planning library or one
    of its dependecies, such as OMPL.  Typically, this means your
    linker is not configured properly.

    Sometimes, you just need to run `ldconfig` or `sudo ldconfig` to
    update the linker cache.

    If this doesn't work, you can set the LD_LIBRARY_PATH variable.
    First, find the location of libompl.so, e.g., by calling `locate
    libompl.so`.  Then, add the directory to your LD_LIBRARY_PATH
    variable.  Most commonly, this will mean adding one of the
    following lines to your shell startup files (e.g., .bashrc):

    `export LD_LIBRARY_PATH="/usr/local/lib/:$LD_LIBRARY_PATH"`

    `export LD_LIBRARY_PATH="/usr/local/lib/x86_64-linux-gnu/:$LD_LIBRARY_PATH"`

* [Contents](@ref faqcontents)

Performance {#performance}
===========

* Q: How do I make it faster?

  - A: Numerical code often benefits from newer CPU instructions. Try
    compiling amino with -march=native either via CFLAGS="-O2
    -march=native ./configure or adding the equivalent to your
    Autoconf site defaults file (config.site).

  - A: An optimized BLAS library will also help some
    operations. OpenBLAS is among the fastest
    (http://www.openblas.net/). If installed, configure amino to use
    it with ./configure --with-blas=openblas.

* Q: Ray Tracing is SLOOOWWW!

  - A: Ray tracing is computationally expensive. Here are a few notes
    to help performance.

    Distribute: Ray tracing is embarassingly parallel, both across
    multiple frames and across pixels within a frame. The easiest way
    to render faster is to throw more cores at the problem.

    Parsing: While POV-Ray can use multiple threads to render a frame,
    parsing the input file uses only a single core. If large meshes
    are used, this can take a noticeable amount of time, and must be
    repeated for each frame. Consequently, rather than using multiple
    threads per frame, it may often be better to render multiple
    frames in parallel so that parsing is parallelized across the
    different frames.

    Optimize POV-Ray: Ray tracing is floating-point heavy. A few
    compilation flags make a small but noticable (~10%) improvement in
    performance.

      * `-march=native`: optimize for the current CPU. This may take
        advantage of FPU instructions available on newer CPUs, e.g.,
        AVX.
      * `-ffast-math`: optimize floating-point computation in ways that
        my violate IEEE semantics. Note that this could flag may
        slightly change the resulting image.
      * `-flto`: enable link-time optimization. This optimizes across
        multiple translation units.

* [Contents](@ref faqcontents)

Comparison with Other Packages    {#comparison}
==============================

* Q: How does Amino relate to MoveIt!?

  - A: MoveIt uses separate data structures for the robot the
    environment.  Amino uses a single data structure (the scene graph)
    for both the robot and environment.  Using a single data structure
    greatly simplifies interactions with multiple objects, e.g.,
    carrying a tray of objects or pushing a loaded cart.

  - A: MoveIt! is closely coupled with ROS.  Amino can load ROS URDF
    files and is also able to operate stand-alone.

* Q: How do the Scene Graphs in Amino compare to ROS URDF?

  - A: URDF and Amino scene graphs contain similar information.  Amino
    can parse URDF files and referenced meshes into scene graphs.

  - A: Amino scene graphs use a streamlined parent-child
    representation of kinematic topology which simplifies modifying
    the kinematic structure, for example, if a robot picks up an
    object or pushes a tray.

* Q: How do the Scene Graphs in Amino compare to the Trees in OROCOS
  KDL?

  - A: Amino provides a more extensive set of kinematic operations
    than KDL, e.g., logarithms, exponentials, derivatives, exact
    integration.

  - A: Amino scene graphs include support for geometry (i.e., meshes,
    primitive shapes) attached to frames and provide visualization
    (via OpenGL and POV-ray) and collision checking (via FCL).
    Geometry is out of scope for KDL.

  - A: Amino scene graphs use a streamlined parent-child
    representation of kinematic topology which simplifies modifying
    the kinematic structure, for example, if a robot picks up an
    object or pushes a tray.

  - A: Amino scene graphs use quaternions which have better
    computational properties that the matrices used in KDL.

* Q: How do the SE(3) functions in amino compare with the Eigen
  Geometry module?

  - A: Aside from the superficial differences in API style, Amino
    provides support for velocities, derivatives, logarithms, and
    exponentials while Eigen does not.  In fact, Eigen quaternions are
    strictly unit quaternions, making it impossible to use them to
    represent derivatives!

  - A: The Eigen Geometry module is composed of C++ templates,
    allowing easy operations on other types (such as single or long
    double floats) but complicating interfacing with languages besides
    C++.  Amino's SE(3) functions are written in C and operate on
    double floats.  These functions are easily called from other
    languages.

* [Contents](@ref faqcontents)
