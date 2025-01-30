/**
 * \mainpage Perception Toolkit
 *
 * The PercepTIon Toolkit is a set of components, each of which provides data
 * structures and  common processing algorithms, for effectively
 * developing the sensing and perception capabilities required for an autonomous
 * system. It aims to provide a cross-platform API that delivers identical
 * behavior on both PC and embedded in order to simplify the
 * development process. Standard tools and techniques can be used to develop and
 * debug on a PC, and the embedded behavior will match. The
 * implementation is standards-compliant C99 and C++11, with additional,
 * architecture-specific high performance routines used transparently where
 * appropriate.
 *
 * Using one component of the PercepTIon Toolkit does not generally require all
 * components to be used, but there is a loose hierarchy (for instance all other
 * components require core due to the basic runtime infrastructure it provides).
 * Component dependencies will be listed with each component. The current
 * components provided by the PercepTIon Toolkit are:
 *
 * - <b>Base Component:</b>
 *   Provides infrastructure and data structures for point, vector, point cloud
 *   and map containers.
 *
 * - <b>Algorithm Component:</b>
 *   Provides API for different sensor specific and sensor independent
 *   algorithms.
 *
 */

/**
 * \defgroup group_ptk_api Perception Toolkit Interface Modules
 */

/**
 * \defgroup group_ptk_base PTK Base
 * \ingroup group_ptk_api
 *
 * - <b>Core:</b>
 *   The core component provides the basic infrastructure that is used by all
 *   other components.
 *
 * - <b>Geometry:</b>
 *   The geometry component provides basic data structures and operations for
 *   manipulating geometric quantities such as points, vectors, and coordinate
 *   systems. These are fundamental elements which are used to build most other
 *   components.
 *
 * - <b>Mapping:</b>
 *   The mapping component provides data structures for generating dense
 *   rectangular maps, with information stored in each map cell, and algorithms
 *   for populating maps from other data structures (e.g. converting a tagged
 *   point cloud into an occupancy grid).
 *
 */

/**
 * \defgroup group_ptk_mapping PTK Mapping Component
 * \ingroup group_ptk_base
 *
 * The mapping component provides data structures for representing information
 * on densely packed grids, where every cell has a value associated with it.
 */

