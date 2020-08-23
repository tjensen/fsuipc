//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// IMplementation of the pyuipc Python module
//-----------------------------------------------------------------------------

#include <algorithm>

#include <windows.h>

#ifdef _WIN64
# include <FSUIPC_User64.h>
#else
# include <FSUIPC_User.h>
#endif

#define _hypot hypot
#include <Python.h>
#include "structmember.h"

//-----------------------------------------------------------------------------

using std::max;
using std::min;

//-----------------------------------------------------------------------------

void* operator new(size_t size)
{
    return malloc(size);
}

//-----------------------------------------------------------------------------

void* operator new[](size_t size)
{
    return malloc(size);
}

//-----------------------------------------------------------------------------

void operator delete(void* ptr)
{
    free(ptr);
}

//-----------------------------------------------------------------------------

void operator delete(void* ptr, std::size_t)
{
    free(ptr);
}

//-----------------------------------------------------------------------------

void operator delete[](void* ptr)
{
    free(ptr);
}

//-----------------------------------------------------------------------------

void operator delete [](void* ptr, std::size_t)
{
    free(ptr);
}

//-----------------------------------------------------------------------------

/**
 * The error strings
 */
const char* errorStrings[] = {
    "OK",
    "Attempt to Open when already Open",
    "Cannot link to FSUIPC or WideClient",
    "Failed to Register common message with Windows",
    "Failed to create Atom for mapping filename",
    "Failed to create a file mapping object",
    "Failed to open a view to the file map",
    "Incorrect version of FSUIPC, or not FSUIPC",
    "Sim is not version requested",
    "Call cannot execute, link not Open",
    "Call cannot execute: no requests accumulated",
    "IPC timed out all retries",
    "IPC sendmessage failed all retries",
    "IPC request contains bad data",
    "Maybe running on WideClient, but FS not running on Server, or wrong FSUIPC",
    "Read or Write request cannot be added, memory for Process is full",
    "Unknown error"
};

static const int numErrorStrings = sizeof(errorStrings)/sizeof(const char*);

//-----------------------------------------------------------------------------

typedef struct {
    PyBaseExceptionObject head;

    int errorCode;
    const char* errorString;
} pyuipc_FSUIPCException;

//-----------------------------------------------------------------------------

static PyTypeObject pyuipc_FSUIPCExceptionType;

//-----------------------------------------------------------------------------

static PyObject* pyuipc_FSUIPCException_new(PyTypeObject* type,
                                            PyObject* args, PyObject* kwds)
{
    return type->tp_base->tp_new(type, args, kwds);
}

//-----------------------------------------------------------------------------

static int pyuipc_FSUIPCException_init(pyuipc_FSUIPCException* self,
                                       PyObject* args, PyObject* kwds)
{
    static char* kwlist[] = {const_cast<char*>("errorCode"), 0};

    int errorCode = -1;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &errorCode)) {
        return -1;
    }

    self->errorCode = errorCode;

    if (errorCode>=numErrorStrings) errorCode = numErrorStrings-1;
    self->errorString = errorStrings[errorCode];

    char msg[256];
    snprintf(msg, sizeof(msg), "FSUIPC error: %d (%s)",
             self->errorCode, self->errorString);

#if PY_MAJOR_VERSION >= 3
    PyObject* message = PyUnicode_FromString(msg);
#else
    PyObject* message = PyString_FromString(msg);
#endif
    args = PyTuple_New(1);
    PyTuple_SET_ITEM(args, 0, message);

    pyuipc_FSUIPCExceptionType.tp_base->tp_init(reinterpret_cast<PyObject*>(self),
                                                args, kwds);

    return 0;
}

//-----------------------------------------------------------------------------

static PyMemberDef pyuipc_FSUIPCException_members[] = {
    {const_cast<char*>("errorCode"), T_INT,
     offsetof(pyuipc_FSUIPCException, errorCode), 0,
     const_cast<char*>("the numeric error code")},
    {const_cast<char*>("errorString"), T_STRING,
     offsetof(pyuipc_FSUIPCException, errorString), 0,
     const_cast<char*>("the string representing the error code")},
    {0,0,0,0,0}
};

//-----------------------------------------------------------------------------

/**
 * Setup the exception type object.
 */
static bool pyuipc_setup_FSUIPCExceptionType()
{
    memset(&pyuipc_FSUIPCExceptionType, 0, sizeof(PyTypeObject));

    PyTypeObject* base = reinterpret_cast<PyTypeObject*>(PyExc_Exception);

#if PY_MAJOR_VERSION >= 3
    pyuipc_FSUIPCExceptionType.ob_base.ob_base.ob_refcnt = 1;
    pyuipc_FSUIPCExceptionType.ob_base.ob_base.ob_type = 0;
    pyuipc_FSUIPCExceptionType.ob_base.ob_size = 0;
#else
    pyuipc_FSUIPCExceptionType.ob_refcnt = 1;
    pyuipc_FSUIPCExceptionType.ob_type = 0;
    pyuipc_FSUIPCExceptionType.ob_size = 0;
#endif
    pyuipc_FSUIPCExceptionType.tp_name = "pyuipc.FSUIPCException";
    pyuipc_FSUIPCExceptionType.tp_basicsize = sizeof(pyuipc_FSUIPCException);
    pyuipc_FSUIPCExceptionType.tp_flags = Py_TPFLAGS_DEFAULT;
    pyuipc_FSUIPCExceptionType.tp_doc = "Exception for FSUIPC errors";
    pyuipc_FSUIPCExceptionType.tp_new = PyType_GenericNew;
    pyuipc_FSUIPCExceptionType.tp_members = pyuipc_FSUIPCException_members;
    pyuipc_FSUIPCExceptionType.tp_new = &pyuipc_FSUIPCException_new;
    pyuipc_FSUIPCExceptionType.tp_init =
        reinterpret_cast<initproc>(&pyuipc_FSUIPCException_init);
    pyuipc_FSUIPCExceptionType.tp_base = base;

    if (PyType_Ready(&pyuipc_FSUIPCExceptionType)<0) {
        printf("Failed readying the exception type");
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------

static void setFSUIPCError(DWORD result)
{
#if PY_MAJOR_VERSION>=3
    PyErr_SetObject(reinterpret_cast<PyObject*>(&pyuipc_FSUIPCExceptionType),
                    PyLong_FromLong(result));
#else
    PyErr_SetObject(reinterpret_cast<PyObject*>(&pyuipc_FSUIPCExceptionType),
                    PyInt_FromLong(result));
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Thread-aware wrappers for the FSUIPC functions

BOOL Py_FSUIPC_Open(DWORD dwFSReq, DWORD* pdwResult)
{
    BOOL retval;
    Py_BEGIN_ALLOW_THREADS
    retval = FSUIPC_Open(dwFSReq, pdwResult);
    Py_END_ALLOW_THREADS
    return retval;
}

//-----------------------------------------------------------------------------

void Py_FSUIPC_Close()
{
    Py_BEGIN_ALLOW_THREADS
    FSUIPC_Close();
    Py_END_ALLOW_THREADS
}

//-----------------------------------------------------------------------------

BOOL Py_FSUIPC_Read(DWORD dwOffset, DWORD dwSize, void* pDest, DWORD* pdwResult)
{
    BOOL retval;
    Py_BEGIN_ALLOW_THREADS
    retval = FSUIPC_Read(dwOffset, dwSize, pDest, pdwResult);
    Py_END_ALLOW_THREADS
    return retval;
}

//-----------------------------------------------------------------------------

BOOL Py_FSUIPC_Write(DWORD dwOffset, DWORD dwSize, void* pSrce, DWORD* pdwResult)
{
    BOOL retval;
    Py_BEGIN_ALLOW_THREADS
    retval = FSUIPC_Write(dwOffset, dwSize, pSrce, pdwResult);
    Py_END_ALLOW_THREADS
    return retval;
}

//-----------------------------------------------------------------------------

BOOL Py_FSUIPC_Process(DWORD* pdwResult)
{
    BOOL retval;
    Py_BEGIN_ALLOW_THREADS
    retval = FSUIPC_Process(pdwResult);
    Py_END_ALLOW_THREADS
    return retval;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * Type of functions that convert bytes received from FSUIPC into a Python
 * object.
 */
typedef PyObject* (bytesToObject_t)(const unsigned char* data);

/**
 * Type of functions that convert a python object into a set of bytes.
 * The return value indicates the success of the conversion.
 */
typedef bool (objectToBytes_t)(unsigned char* data, PyObject* object);

//-----------------------------------------------------------------------------

/**
 * Align the given value so that it is a multiple of alignment
 */
static inline size_t align(size_t value, size_t alignment)
{
    return (value + alignment - 1) & (~(alignment-1));
}

//-----------------------------------------------------------------------------

/**
 * Set an exception with a message.
 */
static void setError(PyObject* exception, char* format, ...)
{
    char buf[256];

    va_list ap;
    va_start(ap, format);

    vsnprintf(buf, sizeof(buf), format, ap);

    va_end(ap);

    PyErr_SetString(exception, buf);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * A template implementation a bytesToObject_t function. It
 * reinterprets the data at the given address as a value of type T and
 * then passes the value to fun, which must return an object.
 */
template <typename T, typename U, PyObject* (fun)(U)>
PyObject* simpleBytesToObject(const unsigned char* data)
{
    return fun(*reinterpret_cast<const T*>(data));
}

//-----------------------------------------------------------------------------

#define DEFINE_SIMPLE_OBJECT_TO_BYTES(letter, T, check, fun, message) \
bool simpleObjectToBytes_##letter(unsigned char* data, PyObject* object) \
{\
    if (!check(object)) {\
        setError(PyExc_TypeError, message);\
        return false;\
    }\
\
    *reinterpret_cast<T*>(data) = static_cast<T>(fun(object));\
    return true;\
}

//-----------------------------------------------------------------------------

#if PY_MAJOR_VERSION>=3

DEFINE_SIMPLE_OBJECT_TO_BYTES(b, unsigned char, PyLong_Check,
                              PyLong_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(c, char, PyLong_Check,
                              PyLong_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(h, short, PyLong_Check,
                              PyLong_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(H, unsigned short, PyLong_Check,
                              PyLong_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(d, int, PyLong_Check,
                              PyLong_AS_LONG, const_cast<char*>("Integer expected"))

#else

DEFINE_SIMPLE_OBJECT_TO_BYTES(b, unsigned char, PyInt_Check,
                              PyInt_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(c, char, PyInt_Check,
                              PyInt_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(h, short, PyInt_Check,
                              PyInt_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(H, unsigned short, PyInt_Check,
                              PyInt_AS_LONG, const_cast<char*>("Integer expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(d, int, PyInt_Check,
                              PyInt_AS_LONG, const_cast<char*>("Integer expected"))

#endif

DEFINE_SIMPLE_OBJECT_TO_BYTES(u, unsigned int, PyLong_Check,
                              PyLong_AsUnsignedLong, const_cast<char*>("Long expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(l, long long, PyLong_Check,
                              PyLong_AsLongLong, const_cast<char*>("Long expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(L, unsigned long long, PyLong_Check,
                              PyLong_AsUnsignedLongLong, const_cast<char*>("Long expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(f, double, PyFloat_Check,
                              PyFloat_AS_DOUBLE, const_cast<char*>("Float expected"))

DEFINE_SIMPLE_OBJECT_TO_BYTES(F, float, PyFloat_Check,
                              PyFloat_AS_DOUBLE, const_cast<char*>("Float expected"))

//-----------------------------------------------------------------------------

// template <typename T, typename U, int (check)(PyObject*),
//           U (fun)(PyObject*), const char* message>
// bool simpleObjectToBytes(unsigned char* data, PyObject* object)
// {
//     if (!check(object)) {
//         setError(PyExc_TypeError, message);
//         return false;
//     }

//     *reinterpret_cast<T*>(data) = fun(object);
// }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * Information about types.
 */
struct TypeInfo {
    /**
     * The static types.
     */
    static const TypeInfo staticTypes[];

    /**
     * Base for the dynamic type infos.
     */
    static const TypeInfo dynamicBase;

    /**
     * Get the static type for the given letter.
     */
    static const TypeInfo* get(char letter);

    /**
     * The letter representing the type
     */
    char letter;

    /**
     * The number of bytes to reserve for values of the type.
     */
    DWORD length;

    /**
     * The alignment of values of the type.
     */
    size_t alignment;

    /**
     * The function that converts a set of bytes into a Python object.
     */
    bytesToObject_t* bytesToObject;

    /**
     * The function that converts a Python object into a set of bytes.
     */
    objectToBytes_t* objectToBytes;

    /**
     * Determine if the type info is dynamic.
     */
    bool isDynamic() const;
};

//-----------------------------------------------------------------------------

/**
 * Types:
 * - b: 1 byte unsigned char -> int
 * - c: 1 byte signed char -> int
 * - h: 2 byte signed short -> int
 * - H: 2 byte unsigned short -> int
 * - d: 4 byte signed integer -> int
 * - u: 4 byte unsigned integer -> long
 * - l: 8 byte signed long long -> long
 * - L: 8 byte unsigned long long -> long
 * - f: 8 byte double -> double
 * - F: 4 byte float -> double
 */
const TypeInfo TypeInfo::staticTypes[] = {
    {'b', 1, 1,
#if PY_MAJOR_VERSION>=3
     &simpleBytesToObject<unsigned char, long, &PyLong_FromLong>,
#else
     &simpleBytesToObject<unsigned char, long, &PyInt_FromLong>,
#endif
     &simpleObjectToBytes_b},
    {'c', 1, 1,
#if PY_MAJOR_VERSION>=3
     &simpleBytesToObject<char, long, &PyLong_FromLong>, &simpleObjectToBytes_c},
#else
     &simpleBytesToObject<char, long, &PyInt_FromLong>, &simpleObjectToBytes_c},
#endif
    {'h', 2, 2,
#if PY_MAJOR_VERSION>=3
     &simpleBytesToObject<short, long, &PyLong_FromLong>, &simpleObjectToBytes_h},
#else
     &simpleBytesToObject<short, long, &PyInt_FromLong>, &simpleObjectToBytes_h},
#endif
    {'H', 2, 2,
#if PY_MAJOR_VERSION>=3
     &simpleBytesToObject<unsigned short, long, &PyLong_FromLong>,
#else
     &simpleBytesToObject<unsigned short, long, &PyInt_FromLong>,
#endif
     &simpleObjectToBytes_H},
    {'d', 4, 4,
#if PY_MAJOR_VERSION>=3
     &simpleBytesToObject<int, long, &PyLong_FromLong>,
#else
     &simpleBytesToObject<int, long, &PyInt_FromLong>,
#endif
     &simpleObjectToBytes_d},
    {'u', 4, 4,
     &simpleBytesToObject<unsigned int, unsigned long,
                          &PyLong_FromUnsignedLong>,
     &simpleObjectToBytes_u},
    {'l', 8, 8,
     &simpleBytesToObject<long long, long long,
                          &PyLong_FromLongLong>,
     &simpleObjectToBytes_l},
    {'L', 8, 8,
     &simpleBytesToObject<unsigned long long, unsigned long long,
                          &PyLong_FromUnsignedLongLong>,
     &simpleObjectToBytes_L},
    {'f', 8, 8,
     &simpleBytesToObject<double, double, &PyFloat_FromDouble>,
      &simpleObjectToBytes_f},
    {'F', 4, 4,
     &simpleBytesToObject<float, double, &PyFloat_FromDouble>,
     &simpleObjectToBytes_F}
};

//-----------------------------------------------------------------------------

const TypeInfo TypeInfo::dynamicBase = {0, 0, 4, 0, 0};

//-----------------------------------------------------------------------------

inline const TypeInfo* TypeInfo::get(char letter)
{
    for(size_t i = 0; i<sizeof(staticTypes)/sizeof(TypeInfo); ++i) {
        const TypeInfo* typeInfo = staticTypes + i;
        if (typeInfo->letter==letter) return typeInfo;
    }
    return 0;
}

//-----------------------------------------------------------------------------

inline bool TypeInfo::isDynamic() const
{
    return letter==0 || letter=='s';
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * Information about a piece of data.
 */
struct DataInfo
{
    /**
     * The offset of the data.
     */
    DWORD offset;

    /**
     * The type information about the data.
     */
    const TypeInfo* typeInfo;

    /**
     * Construct the data info.
     */
    DataInfo(DWORD offset = 0, const TypeInfo* typeInfo = 0);

    /**
     * Destroy the data info.
     */
    ~DataInfo();
};

//-----------------------------------------------------------------------------

inline DataInfo::DataInfo(DWORD offset, const TypeInfo* typeInfo) :
    offset(offset),
    typeInfo(typeInfo)
{
}

//-----------------------------------------------------------------------------

inline DataInfo::~DataInfo()
{
    if (typeInfo!=0 && typeInfo->isDynamic()) delete typeInfo;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * A prepared read/write information.
 */
class PreparedData
{
public:
    /**
     * Destroy the prepared data.
     */
#if PY_MAJOR_VERSION>=3
    static void destroy(PyObject* ptr);
#else
    static void destroy(void* ptr);
#endif

    /**
     * Check if the given Python object is a CObject and if so,
     * extract the pointer from it as a PreparedData pointer.
     */
    static PreparedData* extract(PyObject* object);

private:
    /**
     * Indicate if this data info is for reading/writing or for
     * writing only.
     */
    bool forRead;

    /**
     * The set of type info pointers.
     */
    DataInfo* dataInfo;

    /**
     * The number of data items.
     */
    Py_ssize_t numDataItems;

    /**
     * The data area that we work with.
     */
    unsigned char* data;

public:
    /**
     * Construct the prepated data from the given list of python type
     * specifications.
     *
     * @param forRead if true, the data will be prepared for reading
     *                (as well as writing), i.e. the data area will be
     *                big enough to hold all data. Otherwise it will
     *                only be big enough to hold the largest data.
     */
    PreparedData(PyObject* list, bool forRead = true);

    /**
     * Destroy the prepared data.
     */
    ~PreparedData();

    /**
     * Determine if the data is valid.
     */
    bool isValid() const;

    /**
     * Print the prepared data.
     */
    void print() const;

    /**
     * Issue the read commands to FSUIPC based on the data.
     */
    bool startRead() const;

    /**
     * Process a reading that has been started using startRead(). If
     * the reading is successful, a list will be created with the
     * values read.
     */
    PyObject* processRead() const;

    /**
     * Perform a write from the given list of data. If we are working
     * with a previously prepared data info, the list contains
     * directly the elements. Otherwise each element is a 3-tuple as
     * for a direct write() call: (offset, type, data)
     */
    bool write(PyObject* list, bool prepared) const;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if PY_MAJOR_VERSION>=3

void PreparedData::destroy(PyObject* ptr)
{
    delete reinterpret_cast<PreparedData*>(PyCapsule_GetPointer(ptr, "PreparedData"));
}

#else

void PreparedData::destroy(void* ptr)
{
    delete reinterpret_cast<PreparedData*>(ptr);
}

#endif

//-----------------------------------------------------------------------------

PreparedData* PreparedData::extract(PyObject* object)
{
#if PY_MAJOR_VERSION>=3
    if (PyCapsule_CheckExact(object)) {
        return reinterpret_cast<PreparedData*>(
            PyCapsule_GetPointer(object, "PreparedData"));
#else
    if (PyCObject_Check(object)) {
        return reinterpret_cast<PreparedData*>(PyCObject_AsVoidPtr(object));
#endif
    } else {
        return 0;
    }
}

//-----------------------------------------------------------------------------

PreparedData::PreparedData(PyObject* list, bool forRead) :
    forRead(forRead),
    dataInfo(0),
    numDataItems(0),
    data(0)
{
    //printf("Creating prepared data: %p\n", this);

    if (!PyList_Check(list)) {
        setError(PyExc_TypeError, const_cast<char*>("list is expected"));
        return;
    }

    Py_ssize_t size = PyList_GET_SIZE(list);
    if (size<=0) {
        setError(PyExc_ValueError, const_cast<char*>("list is too short, at least one element is needed"));
        return;
    }

    dataInfo = new DataInfo[size];
    Py_ssize_t i = 0;
    size_t dataSize = 0;
    for(; i<size; ++i) {
        PyObject* item = PyList_GET_ITEM(list, i);
        if (!PyTuple_Check(item) || PyTuple_GET_SIZE(item)<2) {
            setError(PyExc_TypeError, const_cast<char*>("list element %d: should be a tuple of a length of at least 2"), i);
            break;
        }

        PyObject* offset = PyTuple_GET_ITEM(item, 0);
#if PY_MAJOR_VERSION>=3
        if (offset==0 || !PyLong_Check(offset)) {
#else
        if (offset==0 || !PyInt_Check(offset)) {
#endif
            setError(PyExc_TypeError, const_cast<char*>("list element %d: first element of tuple should be an integer"), i);
            break;
        }
#if PY_MAJOR_VERSION>=3
        dataInfo[i].offset = static_cast<size_t>(PyLong_AS_LONG(offset));
#else
        dataInfo[i].offset = static_cast<size_t>(PyInt_AS_LONG(offset));
#endif

        PyObject* type = PyTuple_GET_ITEM(item, 1);

        const TypeInfo* typeInfo = 0;
#if PY_MAJOR_VERSION>=3
        if (PyUnicode_Check(type)) {
            if (PyUnicode_GET_SIZE(type)!=1) {
#else
        if (PyString_Check(type)) {
            if (PyString_GET_SIZE(type)!=1) {
#endif
                setError(PyExc_TypeError, const_cast<char*>("list element %d: type string be of length 1"), i);
                break;
            }
#if PY_MAJOR_VERSION>=3
            char letter = PyUnicode_READ_CHAR(type, 0);
#else
            char letter = PyString_AS_STRING(type)[0];
#endif
            typeInfo = TypeInfo::get(letter);
            if (typeInfo==0) {
                setError(PyExc_TypeError, const_cast<char*>("list element %d: invalid type letter: %c"), i, letter);
                break;
            }
#if PY_MAJOR_VERSION>=3
        } else if (PyLong_Check(type)) {
            long length = static_cast<size_t>(PyLong_AS_LONG(type));
#else
        } else if (PyInt_Check(type)) {
            long length = static_cast<size_t>(PyInt_AS_LONG(type));
#endif
            TypeInfo* ti = new TypeInfo(TypeInfo::dynamicBase);
            if (length<0) ti->letter='s';
            ti->length = (length<0) ? (-length) : length;
            typeInfo = ti;
        } else {
            setError(PyExc_TypeError, const_cast<char*>("list element %d: second element of tuple should be a string or an integer"), i);
            break;
        }

        if (forRead) {
            dataSize = align(dataSize, typeInfo->alignment);
            dataSize += typeInfo->length;
        } else {
            dataSize = max(dataSize, typeInfo->length);
        }

        dataInfo[i].typeInfo = typeInfo;
    }
    if (i<size) {
        delete[] dataInfo;
        dataInfo = 0;
        return;
    }

    numDataItems = size;
    data = new unsigned char[dataSize];
}

//-----------------------------------------------------------------------------

PreparedData::~PreparedData()
{
    //printf("Destroying prepared data: %p\n", this);
    delete[] dataInfo;
    delete[] data;
}

//-----------------------------------------------------------------------------

bool PreparedData::isValid() const
{
    return dataInfo!=0;
}

//-----------------------------------------------------------------------------

void PreparedData::print() const
{
    for(Py_ssize_t i = 0; i<numDataItems; ++i) {
        printf("%02zd: offset=%u, type letter=%c\n",
               i, dataInfo[i].offset, dataInfo[i].typeInfo->letter);
    }
}

//-----------------------------------------------------------------------------

bool PreparedData::startRead() const
{
    if (!forRead) {
        setError(PyExc_ValueError, const_cast<char*>("this prepared data is for writing only"));
        return false;
    }

    Py_ssize_t dataOffset = 0;
    for(Py_ssize_t i = 0; i<numDataItems; ++i) {
        const DataInfo& info = dataInfo[i];
        DWORD result;

        dataOffset = align(dataOffset, info.typeInfo->alignment);

        if (!FSUIPC_Read(info.offset, info.typeInfo->length,
                         data + dataOffset, &result)) {
            setFSUIPCError(result);
            // This hopefully flushes the reads so far
            if (i>0) Py_FSUIPC_Process(&result);
            return false;
        }

//         for(size_t j=0; j<info.typeInfo->length; ++j) {
//             *(data + dataOffset + j) = 'a' + j;
//         }


        dataOffset += info.typeInfo->length;
    }

    return true;
}

//-----------------------------------------------------------------------------

PyObject* PreparedData::processRead() const
{
    DWORD result;
    if (!Py_FSUIPC_Process(&result)) {
        setFSUIPCError(result);
        return 0;
    }

    PyObject* list = PyList_New(numDataItems);

    Py_ssize_t dataOffset = 0;
    for(Py_ssize_t i = 0; i<numDataItems; ++i) {
        const DataInfo& info = dataInfo[i];

        dataOffset = align(dataOffset, info.typeInfo->alignment);

        PyObject* item = 0;
        if (info.typeInfo->isDynamic()) {
            if (info.typeInfo->letter=='s') {
#if PY_MAJOR_VERSION>=3
                item = PyBytes_FromString(reinterpret_cast<const char*>(data + dataOffset));
#else
                item = PyString_FromString(reinterpret_cast<const char*>(data + dataOffset));
#endif
            } else {
#if PY_MAJOR_VERSION>=3
                item = PyBytes_FromStringAndSize(reinterpret_cast<const char*>(data + dataOffset),
                                                 info.typeInfo->length);
#else
                item = PyString_FromStringAndSize(reinterpret_cast<const char*>(data + dataOffset),
                                                  info.typeInfo->length);
#endif
            }
        } else {
            item = info.typeInfo->bytesToObject(data + dataOffset);
        }

        PyList_SET_ITEM(list, i, item);
        dataOffset += info.typeInfo->length;
    }

    return list;
}

//-----------------------------------------------------------------------------

bool PreparedData::write(PyObject* list, bool prepared) const
{
    if (!PyList_Check(list)) {
        setError(PyExc_TypeError, const_cast<char*>("list expected"));
        return false;
    }

    if (PyList_GET_SIZE(list)!=static_cast<int>(numDataItems)) {
        setError(PyExc_TypeError, const_cast<char*>("data list should contain %u elements"),
                 numDataItems);
        return false;
    }

    bool isOK = true;
    for(Py_ssize_t i = 0; i<numDataItems && isOK; ++i) {
        PyObject* item = PyList_GET_ITEM(list, i);
        PyObject* object = 0;
        if (prepared) {
            object = item;
        } else {
            if (!PyTuple_Check(item) || PyTuple_GET_SIZE(item)<3) {
                setError(PyExc_TypeError, const_cast<char*>("the list elements should be tuples with at least 3 elements"));
                isOK = false;
                continue;
            }

            object = PyTuple_GET_ITEM(item, 2);
        }

        const DataInfo& info = dataInfo[i];

        if (info.typeInfo->isDynamic()) {
#if PY_MAJOR_VERSION>=3
            if (!PyBytes_Check(object)) {
#else
            if (!PyString_Check(object)) {
#endif
                setError(PyExc_TypeError,
#if PY_MAJOR_VERSION>=3
                         const_cast<char*>("bytes are expected for generic byte-arrays and strings: offset=0x%04x, type=%s, typeInfo: letter='%c', length=%u, alignment=%u"),
#else
                         const_cast<char*>("a string is expected for generic byte-arrays and strings: offset=0x%04x, type=%s, typeInfo: letter='%c', length=%u, alignment=%u"),
#endif
                         info.offset, Py_TYPE(object)->tp_name,
                         (info.typeInfo->letter==0) ? '@' : info.typeInfo->letter,
                         info.typeInfo->length,
                         info.typeInfo->alignment);
                isOK = false;
                continue;
            }

            if (info.typeInfo->letter=='s') {
                strncpy(reinterpret_cast<char*>(data),
#if PY_MAJOR_VERSION>=3
                        PyBytes_AsString(object),
#else
                        PyString_AS_STRING(object),
#endif
                        info.typeInfo->length);
                data[info.typeInfo->length-1] = '\0';
            } else {
                memset(data, 0, info.typeInfo->length);
#if PY_MAJOR_VERSION>=3
                memcpy(data,
                       PyBytes_AsString(object),
                       min(info.typeInfo->length,
                           static_cast<size_t>(PyBytes_Size(object))));
#else
                memcpy(data, PyString_AS_STRING(object),
                       min(info.typeInfo->length,
                           static_cast<size_t>(PyString_GET_SIZE(object))));
#endif
            }
        } else {
            isOK = info.typeInfo->objectToBytes(data, object);
        }

        if (isOK) {
            DWORD result;
            if (!FSUIPC_Write(info.offset, info.typeInfo->length,
                              data, &result)) {
                setFSUIPCError(result);
                isOK = false;
            }
        }
    }

    // FIXME: what to do if one of the writes fails?
    if (isOK) {
        DWORD result;
        if (!Py_FSUIPC_Process(&result)) {
            setFSUIPCError(result);
            isOK = false;
        }
    }

    return isOK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * Check if args is a tuple and contains count elements
 */
static bool check_argument(PyObject* args, int count = 1)
{
    if (!PyTuple_Check(args)) {
        setError(PyExc_TypeError, const_cast<char*>("tuple expected"));
        return false;
    }

    if (PyTuple_GET_SIZE(args)!=count) {
        setError(PyExc_ValueError, const_cast<char*>("exactly one argument is expected"));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

/**
 * The module.
 */
PyObject* module = 0;

//-----------------------------------------------------------------------------

static PyObject* pyuipc_open(PyObject* /*self*/, PyObject* args)
{
    DWORD fs_req = 0;

    if (!PyArg_ParseTuple(args, "l", &fs_req)) {
        return NULL;
    }

    DWORD result;
    if (Py_FSUIPC_Open(fs_req, &result)) {
#if PY_MAJOR_VERSION>=3
        PyModule_AddObject(module, "fs_version", PyLong_FromLong(FSUIPC_FS_Version));
        PyModule_AddObject(module, "fsuipc_version", PyLong_FromLong(FSUIPC_Version));
        PyModule_AddObject(module, "lib_version", PyLong_FromLong(FSUIPC_Lib_Version));
#else
        PyModule_AddObject(module, "fs_version", PyInt_FromLong(FSUIPC_FS_Version));
        PyModule_AddObject(module, "fsuipc_version", PyInt_FromLong(FSUIPC_Version));
        PyModule_AddObject(module, "lib_version", PyInt_FromLong(FSUIPC_Lib_Version));
#endif
        Py_RETURN_NONE;
    } else {
        setFSUIPCError(result);
        return 0;
    }
}

//-----------------------------------------------------------------------------

static PyObject* pyuipc_prepare_data(PyObject* /*self*/, PyObject* args)
{
    if (!PyTuple_Check(args)) {
        setError(PyExc_TypeError, const_cast<char*>("tuple expected"));
        return 0;
    }

    size_t numArguments = PyTuple_GET_SIZE(args);
    if (numArguments<1 || numArguments>2) {
        setError(PyExc_ValueError, const_cast<char*>("one or two arguments are expected"));
        return 0;
    }

    PyObject* list = PyTuple_GET_ITEM(args, 0);

    bool toRead = true;
    if (numArguments==2) {
        PyObject* forReadObject = PyTuple_GET_ITEM(args, 1);
        if (PyBool_Check(forReadObject)) {
            toRead = forReadObject==Py_True;
#if PY_MAJOR_VERSION>=3
        } else if (PyLong_Check(forReadObject)) {
            toRead = PyLong_AS_LONG(forReadObject)!=0;
#else
        } else if (PyInt_Check(forReadObject)) {
            toRead = PyInt_AS_LONG(forReadObject)!=0;
#endif
        } else {
            setError(PyExc_TypeError, const_cast<char*>("second argument should either be a boolean or an integer"));
            return 0;
        }
    }

    PreparedData* preparedData = new PreparedData(list, toRead);
    if (preparedData->isValid()) {
#if PY_MAJOR_VERSION>=3
        return PyCapsule_New(preparedData, "PreparedData", &PreparedData::destroy);
#else
        return PyCObject_FromVoidPtr(preparedData, &PreparedData::destroy);
#endif
    } else {
        delete preparedData;
        return 0;
    }
}

//-----------------------------------------------------------------------------

static PyObject* pyuipc_print_prepared(PyObject* /*self*/, PyObject* args)
{
    if (!check_argument(args)) return 0;

    PreparedData* data = PreparedData::extract(PyTuple_GET_ITEM(args, 0));
    if (data!=0) {
        data -> print();
        Py_RETURN_NONE;
    } else {
        setError(PyExc_TypeError, const_cast<char*>("a C object is expected"));
        return 0;
    }
 }

//-----------------------------------------------------------------------------

static PyObject* pyuipc_read(PyObject* /*self*/, PyObject* args)
{
    if (!check_argument(args)) return 0;

    PyObject* arg = PyTuple_GET_ITEM(args, 0);

    bool deleteData = false;
    PreparedData* data = PreparedData::extract(arg);
    if (data==0) {
        data = new PreparedData(arg);
        if (!data->isValid()) {
            delete data;
            return 0;
        }
        deleteData = true;
    }

    PyObject* result = 0;
    if (data->startRead()) {
        result = data->processRead();
    }

    if (deleteData) delete data;

    return result;
}

//-----------------------------------------------------------------------------

static PyObject* pyuipc_write(PyObject* /*self*/, PyObject* args)
{
    if (!PyTuple_Check(args)) {
        setError(PyExc_TypeError, const_cast<char*>("tuple expected"));
        return FALSE;
    }

    size_t tupleSize = PyTuple_GET_SIZE(args);
    if (tupleSize<1 || tupleSize>2) {
        setError(PyExc_ValueError, const_cast<char*>("one or two arguments are expected"));
        return FALSE;
    }

    bool prepared = false;
    PreparedData* data = 0;
    PyObject* list = 0;
    if (tupleSize==1) {
        list = PyTuple_GET_ITEM(args, 0);
        data = new PreparedData(list);
        if (!data->isValid()) {
            delete data; data = 0;
        }
    } else {
        data = PreparedData::extract(PyTuple_GET_ITEM(args, 0));
        if (data!=0) {
            prepared = true;
            list = PyTuple_GET_ITEM(args, 1);
        }
    }

    if (data==0 || list==0) {
        setError(PyExc_ValueError, const_cast<char*>("a prepared data is expected"));
        return 0;
    }

    bool isOK = data->write(list, prepared);

    if (!prepared) delete data;

    if (isOK) {
        Py_RETURN_NONE;
    } else {
        return 0;
    }
}

//-----------------------------------------------------------------------------

static PyObject* pyuipc_close(PyObject* /*self*/, PyObject* /*args*/)
{
    Py_FSUIPC_Close();
    Py_RETURN_NONE;
}

//-----------------------------------------------------------------------------

static PyMethodDef PyUIPCMethods[] = {
    {"open",  pyuipc_open, METH_VARARGS, "Open the connection to FSUIPC."},
    {"prepare_data",  pyuipc_prepare_data, METH_VARARGS,
     "Prepare a data specification for reading or writing."},
    {"print_prepared",  pyuipc_print_prepared, METH_VARARGS,
     "Print a prepare a data specification."},
    {"read",  pyuipc_read, METH_VARARGS, "Read some data from FSUIPC."},
    {"write",  pyuipc_write, METH_VARARGS, "Write some data to FSUIPC."},
    {"close",  pyuipc_close, METH_VARARGS, "Close the connection to FSUIPC."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION>=3

static struct PyModuleDef PyUIPCModule = {
    PyModuleDef_HEAD_INIT,
    "pyuipc",
    "Python interface or FSUIPC",
    -1,
    PyUIPCMethods,
    0,
    0,
    0,
    0
};

#endif

PyMODINIT_FUNC
#if PY_MAJOR_VERSION>=3
PyInit_pyuipc(void)
#else
initpyuipc(void)
#endif
{
    if (pyuipc_setup_FSUIPCExceptionType()) {
#if PY_MAJOR_VERSION>=3
        module = PyModule_Create(&PyUIPCModule);
#else
        module = Py_InitModule3("pyuipc", PyUIPCMethods,
                                "Python interface for FSUIPC");
#endif
        Py_INCREF(&pyuipc_FSUIPCExceptionType);
        PyModule_AddObject(module, "FSUIPCException",
                           reinterpret_cast<PyObject*>(&pyuipc_FSUIPCExceptionType));

        PyModule_AddIntConstant(module, "SIM_ANY", 0);
        PyModule_AddIntConstant(module, "SIM_FS98", 1);
        PyModule_AddIntConstant(module, "SIM_FS2K", 2);
        PyModule_AddIntConstant(module, "SIM_CFS2", 3);
        PyModule_AddIntConstant(module, "SIM_CFS1", 4);
        PyModule_AddIntConstant(module, "SIM_FLY", 5);
        PyModule_AddIntConstant(module, "SIM_FS2K2", 6);
        PyModule_AddIntConstant(module, "SIM_FS2K4", 7);
        PyModule_AddIntConstant(module, "SIM_FSX", 8);
        PyModule_AddIntConstant(module, "SIM_ESP", 9);
        PyModule_AddIntConstant(module, "SIM_P3D", 10);
        PyModule_AddIntConstant(module, "SIM_FSX64", 11);
        PyModule_AddIntConstant(module, "SIM_P3D64", 12);
        PyModule_AddIntConstant(module, "SIM_FS2020", 13);

        PyModule_AddIntConstant(module, "ERR_OK", 0);
        PyModule_AddIntConstant(module, "ERR_OPEN", 1);
        PyModule_AddIntConstant(module, "ERR_NOFS", 2);
        PyModule_AddIntConstant(module, "ERR_REGMSG", 3);
        PyModule_AddIntConstant(module, "ERR_ATOM", 4);
        PyModule_AddIntConstant(module, "ERR_MAP", 5);
        PyModule_AddIntConstant(module, "ERR_VIEW", 6);
        PyModule_AddIntConstant(module, "ERR_VERSION", 7);
        PyModule_AddIntConstant(module, "ERR_WRONGFS", 8);
        PyModule_AddIntConstant(module, "ERR_NOTOPEN", 9);
        PyModule_AddIntConstant(module, "ERR_NODATA", 10);
        PyModule_AddIntConstant(module, "ERR_TIMEOUT", 11);
        PyModule_AddIntConstant(module, "ERR_SENDMSG", 12);
        PyModule_AddIntConstant(module, "ERR_DATA", 13);
        PyModule_AddIntConstant(module, "ERR_RUNNING", 14);
        PyModule_AddIntConstant(module, "ERR_SIZE", 15);
    }

#if PY_MAJOR_VERSION>=3
    return module;
#endif
}
