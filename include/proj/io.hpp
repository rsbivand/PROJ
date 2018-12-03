/******************************************************************************
 *
 * Project:  PROJ
 * Purpose:  ISO19111:2018 implementation
 * Author:   Even Rouault <even dot rouault at spatialys dot com>
 *
 ******************************************************************************
 * Copyright (c) 2018, Even Rouault <even dot rouault at spatialys dot com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#ifndef IO_HH_INCLUDED
#define IO_HH_INCLUDED

#include <list>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "util.hpp"

NS_PROJ_START

namespace common {
class UnitOfMeasure;
using UnitOfMeasurePtr = std::shared_ptr<UnitOfMeasure>;
using UnitOfMeasureNNPtr = util::nn<UnitOfMeasurePtr>;

class IdentifiedObject;
using IdentifiedObjectPtr = std::shared_ptr<IdentifiedObject>;
using IdentifiedObjectNNPtr = util::nn<IdentifiedObjectPtr>;
} // namespace common

namespace cs {
class CoordinateSystem;
using CoordinateSystemPtr = std::shared_ptr<CoordinateSystem>;
using CoordinateSystemNNPtr = util::nn<CoordinateSystemPtr>;
} // namespace cs

namespace metadata {
class Extent;
using ExtentPtr = std::shared_ptr<Extent>;
using ExtentNNPtr = util::nn<ExtentPtr>;
} // namespace metadata

namespace datum {
class Datum;
using DatumPtr = std::shared_ptr<Datum>;
using DatumNNPtr = util::nn<DatumPtr>;

class Ellipsoid;
using EllipsoidPtr = std::shared_ptr<Ellipsoid>;
using EllipsoidNNPtr = util::nn<EllipsoidPtr>;

class PrimeMeridian;
using PrimeMeridianPtr = std::shared_ptr<PrimeMeridian>;
using PrimeMeridianNNPtr = util::nn<PrimeMeridianPtr>;

class GeodeticReferenceFrame;
using GeodeticReferenceFramePtr = std::shared_ptr<GeodeticReferenceFrame>;
using GeodeticReferenceFrameNNPtr = util::nn<GeodeticReferenceFramePtr>;

class VerticalReferenceFrame;
using VerticalReferenceFramePtr = std::shared_ptr<VerticalReferenceFrame>;
using VerticalReferenceFrameNNPtr = util::nn<VerticalReferenceFramePtr>;
} // namespace datum

namespace crs {
class CRS;
using CRSPtr = std::shared_ptr<CRS>;
using CRSNNPtr = util::nn<CRSPtr>;

class GeodeticCRS;
using GeodeticCRSPtr = std::shared_ptr<GeodeticCRS>;
using GeodeticCRSNNPtr = util::nn<GeodeticCRSPtr>;

class GeographicCRS;
using GeographicCRSPtr = std::shared_ptr<GeographicCRS>;
using GeographicCRSNNPtr = util::nn<GeographicCRSPtr>;

class VerticalCRS;
using VerticalCRSPtr = std::shared_ptr<VerticalCRS>;
using VerticalCRSNNPtr = util::nn<VerticalCRSPtr>;

class ProjectedCRS;
using ProjectedCRSPtr = std::shared_ptr<ProjectedCRS>;
using ProjectedCRSNNPtr = util::nn<ProjectedCRSPtr>;

class CompoundCRS;
using CompoundCRSPtr = std::shared_ptr<CompoundCRS>;
using CompoundCRSNNPtr = util::nn<CompoundCRSPtr>;
} // namespace crs

namespace operation {
class Conversion;
using ConversionPtr = std::shared_ptr<Conversion>;
using ConversionNNPtr = util::nn<ConversionPtr>;

class CoordinateOperation;
using CoordinateOperationPtr = std::shared_ptr<CoordinateOperation>;
using CoordinateOperationNNPtr = util::nn<CoordinateOperationPtr>;
} // namespace operation

/** osgeo.proj.io namespace.
 *
 * \brief I/O classes
 */
namespace io {

class DatabaseContext;
/** Shared pointer of DatabaseContext. */
using DatabaseContextPtr = std::shared_ptr<DatabaseContext>;
/** Non-null shared pointer of DatabaseContext. */
using DatabaseContextNNPtr = util::nn<DatabaseContextPtr>;

// ---------------------------------------------------------------------------

class WKTNode;
/** Unique pointer of WKTNode. */
using WKTNodePtr = std::unique_ptr<WKTNode>;
/** Non-null unique pointer of WKTNode. */
using WKTNodeNNPtr = util::nn<WKTNodePtr>;

// ---------------------------------------------------------------------------

class WKTFormatter;
/** WKTFormatter unique pointer. */
using WKTFormatterPtr = std::unique_ptr<WKTFormatter>;
/** Non-null WKTFormatter unique pointer. */
using WKTFormatterNNPtr = util::nn<WKTFormatterPtr>;

/** \brief Formatter to WKT strings.
 *
 * An instance of this class can only be used by a single
 * thread at a time.
 */
class PROJ_GCC_DLL WKTFormatter {
  public:
    /** WKT variant. */
    enum class PROJ_MSVC_DLL Convention {
        /** Full WKT2 string, conforming to ISO 19162:2015(E) / OGC 12-063r5
         * (\ref WKT2_2015) with all possible nodes and new keyword names.
         */
        WKT2,
        WKT2_2015 = WKT2,

        /** Same as WKT2 with the following exceptions:
         * <ul>
         *      <li>UNIT keyword used.</li>
         *      <li>ID node only on top element.</li>
         *      <li>No ORDER element in AXIS element.</li>
         *      <li>PRIMEM node omitted if it is Greenwich.</li>
         *      <li>ELLIPSOID.UNIT node omitted if it is
         * UnitOfMeasure::METRE.</li>
         *      <li>PARAMETER.UNIT / PRIMEM.UNIT omitted if same as AXIS.</li>
         *      <li>AXIS.UNIT omitted and replaced by a common GEODCRS.UNIT if
         * they are all the same on all axis.</li>
         * </ul>
         */
        WKT2_SIMPLIFIED,
        WKT2_2015_SIMPLIFIED = WKT2_SIMPLIFIED,

        /** Full WKT2 string, conforming to ISO 19162:2018 / OGC 18-010, with
         * (\ref WKT2_2018) all possible nodes and new keyword names.
         * Non-normative list of differences:
         * <ul>
         *      <li>WKT2_2018 uses GEOGCRS / BASEGEOGCRS keywords for
         * GeographicCRS.</li>
         * </ul>
         */
        WKT2_2018,

        /** WKT2_2018 with the simplification rule of WKT2_SIMPLIFIED */
        WKT2_2018_SIMPLIFIED,

        /** WKT1 as traditionally output by GDAL, deriving from OGC 01-009.
            A notable departuPROJ_GCC_DLLre from WKT1_GDAL with respect to OGC
           01-009 is
            that in WKT1_GDAL, the unit of the PRIMEM value is always degrees.
           */
        WKT1_GDAL,

        /** WKT1 as traditionally output by ESRI software,
         * deriving from OGC 99-049. */
        WKT1_ESRI,
    };

    PROJ_DLL static WKTFormatterNNPtr
    create(Convention convention = Convention::WKT2,
           DatabaseContextPtr dbContext = nullptr);
    PROJ_DLL static WKTFormatterNNPtr create(const WKTFormatterNNPtr &other);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTFormatter();
    //! @endcond

    PROJ_DLL WKTFormatter &setMultiLine(bool multiLine) noexcept;
    PROJ_DLL WKTFormatter &setIndentationWidth(int width) noexcept;

    /** Rule for output AXIS nodes */
    enum class OutputAxisRule {
        /** Always include AXIS nodes */
        YES,
        /** Never include AXIS nodes */
        NO,
        /** Includes them only on PROJCS node if it uses Easting/Northing
         *ordering. Typically used for WKT1_GDAL */
        WKT1_GDAL_EPSG_STYLE,
    };

    PROJ_DLL WKTFormatter &setOutputAxis(OutputAxisRule outputAxis) noexcept;
    PROJ_DLL WKTFormatter &setStrict(bool strict) noexcept;
    PROJ_DLL bool isStrict() const noexcept;

    PROJ_DLL const std::string &toString() const;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        PROJ_DLL WKTFormatter &
        setOutputId(bool outputIdIn);

    PROJ_INTERNAL void enter();
    PROJ_INTERNAL void leave();

    PROJ_INTERNAL void startNode(const std::string &keyword, bool hasId);
    PROJ_INTERNAL void endNode();

    PROJ_DLL WKTFormatter &simulCurNodeHasId();

    PROJ_INTERNAL void addQuotedString(const char *str);
    PROJ_INTERNAL void addQuotedString(const std::string &str);
    PROJ_INTERNAL void add(const std::string &str);
    PROJ_INTERNAL void add(int number);
    PROJ_INTERNAL void add(size_t number) = delete;
    PROJ_INTERNAL void add(double number, int precision = 15);

    PROJ_INTERNAL void pushOutputUnit(bool outputUnitIn);
    PROJ_INTERNAL void popOutputUnit();
    PROJ_INTERNAL bool outputUnit() const;

    PROJ_INTERNAL void pushOutputId(bool outputIdIn);
    PROJ_INTERNAL void popOutputId();
    PROJ_INTERNAL bool outputId() const;

    PROJ_INTERNAL void
    pushAxisLinearUnit(const common::UnitOfMeasureNNPtr &unit);
    PROJ_INTERNAL void popAxisLinearUnit();
    PROJ_INTERNAL const common::UnitOfMeasureNNPtr &axisLinearUnit() const;

    PROJ_INTERNAL void
    pushAxisAngularUnit(const common::UnitOfMeasureNNPtr &unit);
    PROJ_INTERNAL void popAxisAngularUnit();
    PROJ_INTERNAL const common::UnitOfMeasureNNPtr &axisAngularUnit() const;

    PROJ_INTERNAL void setAbridgedTransformation(bool abriged);
    PROJ_INTERNAL bool abridgedTransformation() const;

    PROJ_INTERNAL void setUseDerivingConversion(bool useDerivingConversionIn);
    PROJ_INTERNAL bool useDerivingConversion() const;

    PROJ_INTERNAL void setTOWGS84Parameters(const std::vector<double> &params);
    PROJ_INTERNAL const std::vector<double> &getTOWGS84Parameters() const;

    PROJ_INTERNAL void setVDatumExtension(const std::string &filename);
    PROJ_INTERNAL const std::string &getVDatumExtension() const;

    PROJ_INTERNAL void setHDatumExtension(const std::string &filename);
    PROJ_INTERNAL const std::string &getHDatumExtension() const;

    PROJ_INTERNAL static std::string morphNameToESRI(const std::string &name);

#ifdef unused
    PROJ_INTERNAL void startInversion();
    PROJ_INTERNAL void stopInversion();
    PROJ_INTERNAL bool isInverted() const;
#endif

    PROJ_INTERNAL OutputAxisRule outputAxis() const;
    PROJ_INTERNAL bool outputAxisOrder() const;
    PROJ_INTERNAL bool primeMeridianOmittedIfGreenwich() const;
    PROJ_INTERNAL bool ellipsoidUnitOmittedIfMetre() const;
    PROJ_INTERNAL bool forceUNITKeyword() const;
    PROJ_INTERNAL bool primeMeridianOrParameterUnitOmittedIfSameAsAxis() const;
    PROJ_INTERNAL bool primeMeridianInDegree() const;
    PROJ_INTERNAL bool outputCSUnitOnlyOnceIfSame() const;

    /** WKT version. */
    enum class Version {
        /** WKT1 */
        WKT1,
        /** WKT2 / ISO 19162 */
        WKT2
    };

    PROJ_INTERNAL Version version() const;
    PROJ_INTERNAL bool use2018Keywords() const;
    PROJ_INTERNAL bool useESRIDialect() const;

    PROJ_INTERNAL const DatabaseContextPtr &databaseContext() const;

    PROJ_INTERNAL void ingestWKTNode(const WKTNodeNNPtr &node);

    //! @endcond

  protected:
    //! @cond Doxygen_Suppress
    PROJ_INTERNAL explicit WKTFormatter(Convention convention);
    WKTFormatter(const WKTFormatter &other) = delete;

    INLINED_MAKE_UNIQUE
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

class PROJStringFormatter;
/** PROJStringFormatter unique pointer. */
using PROJStringFormatterPtr = std::unique_ptr<PROJStringFormatter>;
/** Non-null PROJStringFormatter unique pointer. */
using PROJStringFormatterNNPtr = util::nn<PROJStringFormatterPtr>;

/** \brief Formatter to PROJ strings.
 *
 * An instance of this class can only be used by a single
 * thread at a time.
 */
class PROJ_GCC_DLL PROJStringFormatter {
  public:
    /** PROJ variant. */
    enum class PROJ_MSVC_DLL Convention {
        /** PROJ v5 (or later versions) string. */
        PROJ_5,

        /** PROJ v4 string as output by GDAL exportToProj4() */
        PROJ_4
    };

    PROJ_DLL static PROJStringFormatterNNPtr
    create(Convention conventionIn = Convention::PROJ_5,
           DatabaseContextPtr dbContext = nullptr);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~PROJStringFormatter();
    //! @endcond

    PROJ_DLL void setUseETMercForTMerc(bool flag);

    PROJ_DLL const std::string &toString() const;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress

        PROJ_DLL void
        startInversion();
    PROJ_DLL void stopInversion();
    PROJ_INTERNAL bool isInverted() const;
    PROJ_INTERNAL bool getUseETMercForTMerc(bool &settingSetOut) const;
    PROJ_INTERNAL void setCoordinateOperationOptimizations(bool enable);

    PROJ_DLL void
    ingestPROJString(const std::string &str); // throw ParsingException

    PROJ_DLL void addStep(const char *step);
    PROJ_DLL void addStep(const std::string &step);
    PROJ_DLL void setCurrentStepInverted(bool inverted);
    PROJ_DLL void addParam(const std::string &paramName);
    PROJ_DLL void addParam(const char *paramName, double val);
    PROJ_DLL void addParam(const std::string &paramName, double val);
    PROJ_DLL void addParam(const char *paramName, int val);
    PROJ_DLL void addParam(const std::string &paramName, int val);
    PROJ_DLL void addParam(const char *paramName, const char *val);
    PROJ_DLL void addParam(const char *paramName, const std::string &val);
    PROJ_DLL void addParam(const std::string &paramName, const char *val);
    PROJ_DLL void addParam(const std::string &paramName,
                           const std::string &val);
    PROJ_DLL void addParam(const char *paramName,
                           const std::vector<double> &vals);

    PROJ_INTERNAL bool hasParam(const char *paramName) const;

    PROJ_INTERNAL void addNoDefs(bool b);
    PROJ_INTERNAL bool getAddNoDefs() const;

    PROJ_INTERNAL std::set<std::string> getUsedGridNames() const;

    PROJ_INTERNAL void setTOWGS84Parameters(const std::vector<double> &params);
    PROJ_INTERNAL const std::vector<double> &getTOWGS84Parameters() const;

    PROJ_INTERNAL void setVDatumExtension(const std::string &filename);
    PROJ_INTERNAL const std::string &getVDatumExtension() const;

    PROJ_INTERNAL void setHDatumExtension(const std::string &filename);
    PROJ_INTERNAL const std::string &getHDatumExtension() const;

    PROJ_INTERNAL void setOmitProjLongLatIfPossible(bool omit);
    PROJ_INTERNAL bool omitProjLongLatIfPossible() const;

    PROJ_INTERNAL void setOmitZUnitConversion(bool omit);
    PROJ_INTERNAL bool omitZUnitConversion() const;

    PROJ_INTERNAL const DatabaseContextPtr &databaseContext() const;

    PROJ_INTERNAL Convention convention() const;

    //! @endcond

  protected:
    //! @cond Doxygen_Suppress
    PROJ_INTERNAL explicit PROJStringFormatter(
        Convention conventionIn, const DatabaseContextPtr &dbContext);
    PROJStringFormatter(const PROJStringFormatter &other) = delete;

    INLINED_MAKE_UNIQUE
    //! @endcond

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Exception possibly thrown by IWKTExportable::exportToWKT() or
 * IPROJStringExportable::exportToPROJString(). */
class PROJ_GCC_DLL FormattingException : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_INTERNAL explicit FormattingException(const char *message);
    PROJ_INTERNAL explicit FormattingException(const std::string &message);
    PROJ_DLL FormattingException(const FormattingException &other);
    PROJ_DLL virtual ~FormattingException() override;

    PROJ_INTERNAL static void Throw(const char *msg) PROJ_NO_RETURN;
    PROJ_INTERNAL static void Throw(const std::string &msg) PROJ_NO_RETURN;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Exception possibly thrown by WKTNode::createFrom() or
 * WKTParser::createFromWKT(). */
class PROJ_GCC_DLL ParsingException : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_INTERNAL explicit ParsingException(const char *message);
    PROJ_INTERNAL explicit ParsingException(const std::string &message);
    PROJ_DLL ParsingException(const ParsingException &other);
    PROJ_DLL virtual ~ParsingException() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Interface for an object that can be exported to WKT. */
class PROJ_GCC_DLL IWKTExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL virtual ~IWKTExportable();
    //! @endcond

    /** Builds a WKT representation. May throw a FormattingException */
    PROJ_DLL std::string
    exportToWKT(WKTFormatter *formatter) const; // throw(FormattingException)

    PROJ_PRIVATE :

        //! @cond Doxygen_Suppress
        PROJ_INTERNAL virtual void
        _exportToWKT(
            WKTFormatter *formatter) const = 0; // throw(FormattingException)
    //! @endcond
};

// ---------------------------------------------------------------------------

class IPROJStringExportable;
/** Shared pointer of IPROJStringExportable. */
using IPROJStringExportablePtr = std::shared_ptr<IPROJStringExportable>;
/** Non-null shared pointer of IPROJStringExportable. */
using IPROJStringExportableNNPtr = util::nn<IPROJStringExportablePtr>;

/** \brief Interface for an object that can be exported to a PROJ string. */
class PROJ_GCC_DLL IPROJStringExportable {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL virtual ~IPROJStringExportable();
    //! @endcond

    /** \brief Builds a PROJ string representation.
     *
     * <ul>
     * <li>For PROJStringFormatter::Convention::PROJ_5 (the default), return
     * strings that generally express PROJ.5 pipelines.
     * <ul>
     * <li>For a crs::GeographicCRS, returns a string expressing the
     * transformation from geographic coordinates expressed in radian with
     * longitude, latitude order, and with respect to the international
     * reference meridian, into geographic coordinates expressed in the units
     * and axis order of the CRS, taking into account its prime meridian.</li>
     * <li>For a geocentric crs::GeodeticCRS, returns a string expressing the
     * transformation from geographic coordinates expressed in radian with
     * longitude, latitude order, and with respect to the international
     * reference meridian, into geocentric coordinates.</li>
     * <li>For a
     * crs::ProjectedCRS / crs::DerivedGeographicCRS / crs::DerivedProjectedCRS,
     * returns a string expressing the transformation from the base CRS to the
     * CRS</li>
     * <li>For a crs::BoundCRS, throws a FormattingException.</li>
     * <li>For operation::CoordinateTransformations, returns a PROJ
     * pipeline.</li>
     * </ul>
     *
     * <li>For PROJStringFormatter::Convention::PROJ_4, format a string
     * compatible with the OGRSpatialReference::exportToProj4() of GDAL
     * &lt;=2.3.
     * <ul>
     * <li>For a crs::GeographicCRS, returns a proj=longlat string, with
     * ellipsoid / datum / prime meridian information, ignoring axis order
     * and unit information.</li>
     * <li>For a geocentric crs::GeodeticCRS, returns the transformation from
     * geographic coordinates into geocentric coordinates.</li>
     * <li>For a crs::ProjectedCRS, returns the projection method, ignoring
     * axis order.</li>
     * <li>For a crs::BoundCRS, returns the PROJ string of its source/base CRS,
     * amended with towgs84 / nadgrids parameter when the deriving conversion
     * can be expressed in that way.</li>
     * </ul>
     * </li>
     *
     * </ul>
     *
     * @param formatter PROJ string formatter.
     * @return a PROJ string.
     * @throw FormattingException */
    PROJ_DLL std::string exportToPROJString(
        PROJStringFormatter *formatter) const; // throw(FormattingException)

    PROJ_PRIVATE :

        //! @cond Doxygen_Suppress
        PROJ_INTERNAL virtual void
        _exportToPROJString(PROJStringFormatter *formatter)
            const = 0; // throw(FormattingException)
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Node in the tree-splitted WKT representation.
 */
class PROJ_GCC_DLL WKTNode {
  public:
    PROJ_DLL explicit WKTNode(const std::string &valueIn);
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTNode();
    //! @endcond

    PROJ_DLL const std::string &value() const;
    PROJ_DLL const std::vector<WKTNodeNNPtr> &children() const;

    PROJ_DLL void addChild(WKTNodeNNPtr &&child);
    PROJ_DLL const WKTNodePtr &lookForChild(const std::string &childName,
                                            int occurrence = 0) const noexcept;
    PROJ_DLL int countChildrenOfName(const std::string &childName) const
        noexcept;

    PROJ_DLL std::string toString() const;

    PROJ_DLL static WKTNodeNNPtr createFrom(const std::string &wkt,
                                            size_t indexStart = 0);

  protected:
    PROJ_INTERNAL static WKTNodeNNPtr
    createFrom(const std::string &wkt, size_t indexStart, int recLevel,
               size_t &indexEnd); // throw(ParsingException)

  private:
    friend class WKTParser;
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

PROJ_DLL util::BaseObjectNNPtr
createFromUserInput(const std::string &text,
                    const DatabaseContextPtr &dbContext,
                    bool usePROJ4InitRules = false);

// ---------------------------------------------------------------------------

/** \brief Parse a WKT string into the appropriate suclass of util::BaseObject.
 */
class PROJ_GCC_DLL WKTParser {
  public:
    PROJ_DLL WKTParser();
    //! @cond Doxygen_Suppress
    PROJ_DLL ~WKTParser();
    //! @endcond

    PROJ_DLL WKTParser &
    attachDatabaseContext(const DatabaseContextPtr &dbContext);

    PROJ_DLL WKTParser &setStrict(bool strict);
    PROJ_DLL std::vector<std::string> warningList() const;

    PROJ_DLL util::BaseObjectNNPtr
    createFromWKT(const std::string &wkt); // throw(ParsingException)

    /** Guessed WKT "dialect" */
    enum class PROJ_MSVC_DLL WKTGuessedDialect {
        /** \ref WKT2_2018 */
        WKT2_2018,
        /** \ref WKT2_2015 */
        WKT2_2015,
        /** \ref WKT1 */
        WKT1_GDAL,
        /** ESRI variant of WKT1 */
        WKT1_ESRI,
        /** Not WKT / unrecognized */
        NOT_WKT
    };

    // cppcheck-suppress functionStatic
    PROJ_DLL WKTGuessedDialect guessDialect(const std::string &wkt) noexcept;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Parse a PROJ string into the appropriate suclass of util::BaseObject.
 */
class PROJ_GCC_DLL PROJStringParser {
  public:
    PROJ_DLL PROJStringParser();
    //! @cond Doxygen_Suppress
    PROJ_DLL ~PROJStringParser();
    //! @endcond

    PROJ_DLL PROJStringParser &
    attachDatabaseContext(const DatabaseContextPtr &dbContext);

    PROJ_DLL PROJStringParser &setUsePROJ4InitRules(bool enable);

    PROJ_DLL std::vector<std::string> warningList() const;

    PROJ_DLL util::BaseObjectNNPtr createFromPROJString(
        const std::string &projString); // throw(ParsingException)

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Database context.
 *
 * A database context should be used only by one thread at a time.
 */
class PROJ_GCC_DLL DatabaseContext {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~DatabaseContext();
    //! @endcond

    PROJ_DLL static DatabaseContextNNPtr create();

    PROJ_DLL static DatabaseContextNNPtr
    create(const std::string &databasePath);

    PROJ_DLL static DatabaseContextNNPtr
    create(const std::string &databasePath,
           const std::vector<std::string> &auxiliaryDatabasePaths);

    PROJ_DLL const std::string &getPath() const;

    PROJ_DLL const char *getMetadata(const char *key) const;

    PROJ_DLL std::set<std::string> getAuthorities() const;

    PROJ_DLL std::vector<std::string> getDatabaseStructure() const;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress
        PROJ_DLL void *
        getSqliteHandle() const;

    PROJ_DLL static DatabaseContextNNPtr create(void *sqlite_handle);

    PROJ_INTERNAL void attachPJContext(void *pjCtxt);

    PROJ_INTERNAL bool lookForGridAlternative(const std::string &officialName,
                                              std::string &projFilename,
                                              std::string &projFormat,
                                              bool &inverse) const;

    PROJ_DLL bool lookForGridInfo(const std::string &projFilename,
                                  std::string &fullFilename,
                                  std::string &packageName, std::string &url,
                                  bool &directDownload, bool &openLicense,
                                  bool &gridAvailable) const;

    PROJ_INTERNAL std::string
    getAliasFromOfficialName(const std::string &officialName,
                             const std::string &tableName,
                             const std::string &source) const;

    PROJ_INTERNAL bool isKnownName(const std::string &name,
                                   const std::string &tableName) const;

    PROJ_INTERNAL std::string getTextDefinition(const std::string &tableName,
                                                const std::string &authName,
                                                const std::string &code) const;
    //! @endcond

  protected:
    PROJ_INTERNAL DatabaseContext();
    INLINED_MAKE_SHARED
    PROJ_FRIEND(AuthorityFactory);

  private:
    PROJ_OPAQUE_PRIVATE_DATA
    DatabaseContext(const DatabaseContext &) = delete;
    DatabaseContext &operator=(const DatabaseContext &other) = delete;
};

// ---------------------------------------------------------------------------

class AuthorityFactory;
/** Shared pointer of AuthorityFactory. */
using AuthorityFactoryPtr = std::shared_ptr<AuthorityFactory>;
/** Non-null shared pointer of AuthorityFactory. */
using AuthorityFactoryNNPtr = util::nn<AuthorityFactoryPtr>;

/** \brief Builds object from an authority database.
 *
 * A AuthorityFactory should be used only by one thread at a time.
 *
 * \remark Implements [AuthorityFactory]
 * (http://www.geoapi.org/3.0/javadoc/org/opengis/referencing/AuthorityFactory.html)
 * from \ref GeoAPI
 */
class PROJ_GCC_DLL AuthorityFactory {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL ~AuthorityFactory();
    //! @endcond

    PROJ_DLL util::BaseObjectNNPtr createObject(const std::string &code) const;

    PROJ_DLL common::UnitOfMeasureNNPtr
    createUnitOfMeasure(const std::string &code) const;

    PROJ_DLL metadata::ExtentNNPtr createExtent(const std::string &code) const;

    PROJ_DLL datum::PrimeMeridianNNPtr
    createPrimeMeridian(const std::string &code) const;

    PROJ_DLL std::string identifyBodyFromSemiMajorAxis(double a,
                                                       double tolerance) const;

    PROJ_DLL datum::EllipsoidNNPtr
    createEllipsoid(const std::string &code) const;

    PROJ_DLL datum::DatumNNPtr createDatum(const std::string &code) const;

    PROJ_DLL datum::GeodeticReferenceFrameNNPtr
    createGeodeticDatum(const std::string &code) const;

    PROJ_DLL datum::VerticalReferenceFrameNNPtr
    createVerticalDatum(const std::string &code) const;

    PROJ_DLL cs::CoordinateSystemNNPtr
    createCoordinateSystem(const std::string &code) const;

    PROJ_DLL crs::GeodeticCRSNNPtr
    createGeodeticCRS(const std::string &code) const;

    PROJ_DLL crs::GeographicCRSNNPtr
    createGeographicCRS(const std::string &code) const;

    PROJ_DLL crs::VerticalCRSNNPtr
    createVerticalCRS(const std::string &code) const;

    PROJ_DLL operation::ConversionNNPtr
    createConversion(const std::string &code) const;

    PROJ_DLL crs::ProjectedCRSNNPtr
    createProjectedCRS(const std::string &code) const;

    PROJ_DLL crs::CompoundCRSNNPtr
    createCompoundCRS(const std::string &code) const;

    PROJ_DLL crs::CRSNNPtr
    createCoordinateReferenceSystem(const std::string &code) const;

    PROJ_DLL operation::CoordinateOperationNNPtr
    createCoordinateOperation(const std::string &code,
                              bool usePROJAlternativeGridNames) const;

    PROJ_DLL std::vector<operation::CoordinateOperationNNPtr>
    createFromCoordinateReferenceSystemCodes(
        const std::string &sourceCRSCode,
        const std::string &targetCRSCode) const;

    PROJ_DLL const std::string &getAuthority() PROJ_CONST_DECL;

    /** Object type. */
    enum class ObjectType {
        /** Object of type datum::PrimeMeridian */
        PRIME_MERIDIAN,
        /** Object of type datum::Ellipsoid */
        ELLIPSOID,
        /** Object of type datum::Datum (and derived classes) */
        DATUM,
        /** Object of type datum::GeodeticReferenceFrame (and derived
           classes) */
        GEODETIC_REFERENCE_FRAME,
        /** Object of type datum::VerticalReferenceFrame (and derived
           classes) */
        VERTICAL_REFERENCE_FRAME,
        /** Object of type crs::CRS (and derived classes) */
        CRS,
        /** Object of type crs::GeodeticCRS (and derived classes) */
        GEODETIC_CRS,
        /** GEODETIC_CRS of type geocentric */
        GEOCENTRIC_CRS,
        /** Object of type crs::GeographicCRS (and derived classes) */
        GEOGRAPHIC_CRS,
        /** GEOGRAPHIC_CRS of type Geographic 2D */
        GEOGRAPHIC_2D_CRS,
        /** GEOGRAPHIC_CRS of type Geographic 3D */
        GEOGRAPHIC_3D_CRS,
        /** Object of type crs::ProjectedCRS (and derived classes) */
        PROJECTED_CRS,
        /** Object of type crs::VerticalCRS (and derived classes) */
        VERTICAL_CRS,
        /** Object of type crs::CompoundCRS (and derived classes) */
        COMPOUND_CRS,
        /** Object of type operation::CoordinateOperation (and derived
           classes) */
        COORDINATE_OPERATION,
        /** Object of type operation::Conversion (and derived classes) */
        CONVERSION,
        /** Object of type operation::Transformation (and derived classes)
         */
        TRANSFORMATION,
        /** Object of type operation::ConcatenatedOperation (and derived
           classes) */
        CONCATENATED_OPERATION,
    };

    PROJ_DLL std::set<std::string>
    getAuthorityCodes(const ObjectType &type,
                      bool allowDeprecated = true) const;

    PROJ_DLL std::string getDescriptionText(const std::string &code) const;

    // non-standard
    PROJ_DLL static AuthorityFactoryNNPtr
    create(const DatabaseContextNNPtr &context,
           const std::string &authorityName);

    PROJ_DLL const DatabaseContextNNPtr &databaseContext() const;

    PROJ_DLL std::vector<operation::CoordinateOperationNNPtr>
    createFromCoordinateReferenceSystemCodes(
        const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
        const std::string &targetCRSAuthName, const std::string &targetCRSCode,
        bool usePROJAlternativeGridNames, bool discardIfMissingGrid) const;

    PROJ_DLL std::vector<operation::CoordinateOperationNNPtr>
    createFromCRSCodesWithIntermediates(
        const std::string &sourceCRSAuthName, const std::string &sourceCRSCode,
        const std::string &targetCRSAuthName, const std::string &targetCRSCode,
        bool usePROJAlternativeGridNames, bool discardIfMissingGrid,
        const std::vector<std::pair<std::string, std::string>>
            &intermediateCRSAuthCodes) const;

    PROJ_DLL std::string getOfficialNameFromAlias(
        const std::string &aliasedName, const std::string &tableName,
        const std::string &source, std::string &outTableName,
        std::string &outAuthName, std::string &outCode) const;

    PROJ_DLL std::list<common::IdentifiedObjectNNPtr>
    createObjectsFromName(const std::string &name,
                          const std::vector<ObjectType> &allowedObjectTypes =
                              std::vector<ObjectType>(),
                          bool approximateMatch = true,
                          size_t limitResultCount = 0) const;

    PROJ_DLL std::list<std::pair<std::string, std::string>>
    listAreaOfUseFromName(const std::string &name, bool approximateMatch) const;

    PROJ_PRIVATE :
        //! @cond Doxygen_Suppress

        PROJ_INTERNAL std::list<datum::EllipsoidNNPtr>
        createEllipsoidFromExisting(
            const datum::EllipsoidNNPtr &ellipsoid) const;

    PROJ_INTERNAL std::list<crs::GeodeticCRSNNPtr>
    createGeodeticCRSFromDatum(const std::string &datum_auth_name,
                               const std::string &datum_code,
                               const std::string &geodetic_crs_type) const;

    PROJ_INTERNAL std::list<crs::GeodeticCRSNNPtr>
    createGeodeticCRSFromEllipsoid(const std::string &ellipsoid_auth_name,
                                   const std::string &ellipsoid_code,
                                   const std::string &geodetic_crs_type) const;

    PROJ_INTERNAL std::list<crs::ProjectedCRSNNPtr>
    createProjectedCRSFromExisting(const crs::ProjectedCRSNNPtr &crs) const;

    PROJ_INTERNAL std::list<crs::CompoundCRSNNPtr>
    createCompoundCRSFromExisting(const crs::CompoundCRSNNPtr &crs) const;
    //! @endcond

  protected:
    PROJ_INTERNAL AuthorityFactory(const DatabaseContextNNPtr &context,
                                   const std::string &authorityName);

    PROJ_INTERNAL crs::CRSNNPtr
    createCoordinateReferenceSystem(const std::string &code,
                                    bool allowCompound) const;

    PROJ_INTERNAL crs::GeodeticCRSNNPtr
    createGeodeticCRS(const std::string &code, bool geographicOnly) const;

    PROJ_INTERNAL operation::CoordinateOperationNNPtr
    createCoordinateOperation(const std::string &code, bool allowConcatenated,
                              bool usePROJAlternativeGridNames) const;

    INLINED_MAKE_SHARED

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

// ---------------------------------------------------------------------------

/** \brief Exception thrown when a factory can't create an instance of the
 * requested object.
 */
class PROJ_GCC_DLL FactoryException : public util::Exception {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit FactoryException(const char *message);
    PROJ_DLL explicit FactoryException(const std::string &message);
    PROJ_DLL
    FactoryException(const FactoryException &other);
    PROJ_DLL ~FactoryException() override;
    //! @endcond
};

// ---------------------------------------------------------------------------

/** \brief Exception thrown when an authority factory can't find the requested
 * authority code.
 */
class PROJ_GCC_DLL NoSuchAuthorityCodeException : public FactoryException {
  public:
    //! @cond Doxygen_Suppress
    PROJ_DLL explicit NoSuchAuthorityCodeException(const std::string &message,
                                                   const std::string &authority,
                                                   const std::string &code);
    PROJ_DLL
    NoSuchAuthorityCodeException(const NoSuchAuthorityCodeException &other);
    PROJ_DLL ~NoSuchAuthorityCodeException() override;
    //! @endcond

    PROJ_DLL const std::string &getAuthority() const;
    PROJ_DLL const std::string &getAuthorityCode() const;

  private:
    PROJ_OPAQUE_PRIVATE_DATA
};

} // namespace io

NS_PROJ_END

#endif // IO_HH_INCLUDED
