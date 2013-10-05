
/*
Copyright (c) 2013 Khaled Mammou - Advanced Micro Devices, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

var o3dgc = (function (module) {
    var module = {};

    var O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0 = 7;
    var O3DGC_BINARY_STREAM_MAX_SYMBOL0 = ((1 << O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0) >>> 0) - 1;
    var O3DGC_BINARY_STREAM_BITS_PER_SYMBOL1 = 6;
    var O3DGC_BINARY_STREAM_MAX_SYMBOL1 = ((1 << O3DGC_BINARY_STREAM_BITS_PER_SYMBOL1) >>> 0) - 1;
    var O3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32 = Math.floor((32 + O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0 - 1) / O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0);
    var O3DGC_BIG_ENDIAN = 0;
    var O3DGC_LITTLE_ENDIAN = 1;
    var O3DGC_MAX_DOUBLE = 1.79769e+308;
    var O3DGC_MIN_LONG = -2147483647;
    var O3DGC_MAX_LONG = 2147483647;
    var O3DGC_MAX_UCHAR8 = 255;
    var O3DGC_MAX_TFAN_SIZE = 256;
    var O3DGC_MAX_ULONG = 4294967295;
    var O3DGC_SC3DMC_START_CODE = 0x00001F1;
    var O3DGC_DV_START_CODE = 0x00001F2;
    var O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES = 256;
    var O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES = 256;
    var O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES = 32;
    var O3DGC_SC3DMC_MAX_DIM_INT_ATTRIBUTES = 32;
    var O3DGC_SC3DMC_MAX_PREDICTION_NEIGHBORS = 8;
    var O3DGC_OK = 0;
    var O3DGC_ERROR_BUFFER_FULL = 1;
    var O3DGC_ERROR_CORRUPTED_STREAM = 5;
    var O3DGC_ERROR_NON_SUPPORTED_FEATURE = 6;
    var O3DGC_ERROR_AC = 7;
    var O3DGC_SC3DMC_BINARIZATION_FL = 0; // Fixed Length (not supported)
    var O3DGC_SC3DMC_BINARIZATION_BP = 1; // BPC (not supported)
    var O3DGC_SC3DMC_BINARIZATION_FC = 2; // 4 bits Coding (not supported)
    var O3DGC_SC3DMC_BINARIZATION_AC = 3; // Arithmetic Coding (not supported)
    var O3DGC_SC3DMC_BINARIZATION_AC_EGC = 4; // Arithmetic Coding & EGCk
    var O3DGC_SC3DMC_BINARIZATION_ASCII = 5; // Arithmetic Coding & EGCk
    var O3DGC_STREAM_TYPE_UNKOWN = 0;
    var O3DGC_STREAM_TYPE_ASCII = 1;
    var O3DGC_STREAM_TYPE_BINARY = 2;
    var O3DGC_SC3DMC_NO_PREDICTION = 0; // supported
    var O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION = 1; // supported
    var O3DGC_SC3DMC_XOR_PREDICTION = 2; // not supported
    var O3DGC_SC3DMC_ADAPTIVE_DIFFERENTIAL_PREDICTION = 3; // not supported
    var O3DGC_SC3DMC_CIRCULAR_DIFFERENTIAL_PREDICTION = 4; // not supported
    var O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION = 5; // supported
    var O3DGC_SC3DMC_SURF_NORMALS_PREDICTION = 6; // supported
    var O3DGC_SC3DMC_QBCR = 0; // not supported
    var O3DGC_SC3DMC_SVA = 1; // not supported
    var O3DGC_SC3DMC_TFAN = 2; // supported
    var O3DGC_DYNAMIC_VECTOR_LIFT = 0;
    var O3DGC_MIN_NEIGHBORS_SIZE = 128;
    var O3DGC_MIN_NUM_NEIGHBORS_SIZE = 16;
    var O3DGC_TFANS_MIN_SIZE_ALLOCATED_VERTICES_BUFFER = 128;
    var O3DGC_TFANS_MIN_SIZE_TFAN_SIZE_BUFFER = 8;
    var O3DGC_DEFAULT_VECTOR_SIZE = 32;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_UNKOWN = 0;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_POSITION = 1;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_NORMAL = 2;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_COLOR = 3;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD = 4;
    module.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT = 5;
    module.O3DGC_IFS_INT_ATTRIBUTE_TYPE_UNKOWN = 0;
    module.O3DGC_IFS_INT_ATTRIBUTE_TYPE_INDEX = 1;
    module.O3DGC_IFS_INT_ATTRIBUTE_TYPE_JOINT_ID = 2;
    module.O3DGC_IFS_INT_ATTRIBUTE_TYPE_INDEX_BUFFER_ID = 3;

    function SystemEndianness() {
        var b = new ArrayBuffer(4);
        var a = new Uint32Array(b);
        var c = new Uint8Array(b);
        a[0] = 1;
        if (c[0] === 1) {
            return O3DGC_LITTLE_ENDIAN;
        }
        else {
            return O3DGC_BIG_ENDIAN;
        }
    }
    // SC3DMCStats class
    module.SC3DMCStats = function () {
        this.m_timeCoord = 0;
        this.m_timeNormal = 0;
        this.m_timeCoordIndex = 0;
        this.m_timeFloatAttribute = new Float32Array(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
        this.m_timeIntAttribute = new Float32Array(O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
        this.m_timeReorder = 0;
        this.m_streamSizeCoord = 0;
        this.m_streamSizeNormal = 0;
        this.m_streamSizeCoordIndex = 0;
        this.m_streamSizeFloatAttribute = new Uint32Array(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
        this.m_streamSizeIntAttribute = new Uint32Array(O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
    }
    // SC3DMCTriplet class
    module.SC3DMCTriplet = function (a, b, c) {
        this.m_a = a;
        this.m_b = b;
        this.m_c = c;
    }
    module.SC3DMCTriplet.prototype.Less = function (rhs) {
        if (this.m_c !== rhs.m_c) {
            return (this.m_c < rhs.m_c);
        }
        else if (this.m_b !== rhs.m_b) {
            return (this.m_b < rhs.m_b);
        }
        return (this.m_a < rhs.m_a);
    }
    module.SC3DMCTriplet.prototype.Equal = function (rhs) {
        return (this.m_c === rhs.m_c && this.m_b === rhs.m_b && this.m_a === rhs.m_a);
    }
    // SC3DMCPredictor class
    module.SC3DMCPredictor = function () {
        this.m_id = new module.SC3DMCTriplet(-1, -1, -1);
        this.m_pred = new Float32Array(O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
    }
    // fix me: optimize this function (e.g., binary search)
    function InsertPredictor(e, nPred, list, dimFloatArray) {
        var pos = 0xFFFFFFFF;
        var foundOrInserted = false;
        for (var j = 0; j < nPred.m_value; ++j) {
            if (e.Equal(list[j].m_id)) {
                foundOrInserted = true;
                break;
            }
            else if (e.Less(list[j].m_id)) {
                if (nPred.m_value < O3DGC_SC3DMC_MAX_PREDICTION_NEIGHBORS) {
                    ++nPred.m_value;
                }
                for (var h = nPred.m_value - 1; h > j; --h) {
                    list[h].m_id.m_a = list[h - 1].m_id.m_a;
                    list[h].m_id.m_b = list[h - 1].m_id.m_b;
                    list[h].m_id.m_c = list[h - 1].m_id.m_c;
                    for (var i = 0; i < dimFloatArray; ++i) {
                        list[h].m_pred[i] = list[h - 1].m_pred[i];
                    }
                }
                list[j].m_id.m_a = e.m_a;
                list[j].m_id.m_b = e.m_b;
                list[j].m_id.m_c = e.m_c;
                pos = j;
                foundOrInserted = true;
                break;
            }
        }
        if (!foundOrInserted && nPred.m_value < O3DGC_SC3DMC_MAX_PREDICTION_NEIGHBORS) {
            pos = nPred.m_value++;
            list[pos].m_id.m_a = e.m_a;
            list[pos].m_id.m_b = e.m_b;
            list[pos].m_id.m_c = e.m_c;
        }
        return pos;
    }
    // Timer class
    (function () {
        if (typeof window.performance === 'undefined') {
            window.performance = {};
        }
        if (!window.performance.now) {
            var nowOffset = Date.now();
            if (performance.timing && performance.timing.navigationStart) {
                nowOffset = performance.timing.navigationStart
            }
            window.performance.now = function now() {
                return Date.now() - nowOffset;
            }
        }
    })();
    module.Timer = function () {
        this.m_start = 0;
        this.m_end = 0;
    }
    module.Timer.prototype.Tic = function () {
        this.m_start = window.performance.now();
    }
    module.Timer.prototype.Toc = function () {
        this.m_end = window.performance.now();
    }
    module.Timer.prototype.GetElapsedTime = function () {
        return this.m_end - this.m_start;
    }
    // Vec3 class
    module.Vec3 = function (x, y, z) {
        this.m_x = x;
        this.m_y = y;
        this.m_z = z;
    }
    module.Vec3.prototype.Set = function (x, y, z) {
        this.m_x = x;
        this.m_y = y;
        this.m_z = z;
    }
    module.Vec3.prototype.Sub = function (lhs, rhs) {
        this.m_x = lhs.m_x - rhs.m_x;
        this.m_y = lhs.m_y - rhs.m_y;
        this.m_z = lhs.m_z - rhs.m_z;
    }
    module.Vec3.prototype.Add = function (lhs, rhs) {
        this.m_x = lhs.m_x + rhs.m_x;
        this.m_y = lhs.m_y + rhs.m_y;
        this.m_z = lhs.m_z + rhs.m_z;
    }
    module.Vec3.prototype.SelfAdd = function (v) {
        this.m_x += v.m_x;
        this.m_y += v.m_y;
        this.m_z += v.m_z;
    }
    module.Vec3.prototype.Cross = function (lhs, rhs) {
        this.m_x = lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y;
        this.m_y = lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z;
        this.m_z = lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x;
    }
    module.Vec3.prototype.GetNorm = function () {
        return Math.sqrt(this.m_x * this.m_x + this.m_y * this.m_y + this.m_z * this.m_z);
    }
    function SphereToCube(vin, vout) {
        var ax = Math.abs(vin.m_x);
        var ay = Math.abs(vin.m_y);
        var az = Math.abs(vin.m_z);
        if (az >= ax && az >= ay) {
            if (vin.m_z >= 0) {
                vout.m_z = 0;
                vout.m_x = vin.m_x;
                vout.m_y = vin.m_y;
            }
            else {
                vout.m_z = 1;
                vout.m_x = -vin.m_x;
                vout.m_y = -vin.m_y;
            }
        }
        else if (ay >= ax && ay >= az) {
            if (vin.m_y >= 0) {
                vout.m_z = 2;
                vout.m_x = vin.m_z;
                vout.m_y = vin.m_x;
            }
            else {
                vout.m_z = 3;
                vout.m_x = -vin.m_z;
                vout.m_y = -vin.m_x;
            }
        }
        else {
            if (vin.m_x >= 0) {
                vout.m_z = 4;
                vout.m_x = vin.m_y;
                vout.m_y = vin.m_z;
            }
            else {
                vout.m_z = 5;
                vout.m_x = -vin.m_y;
                vout.m_y = -vin.m_z;
            }
        }
    }
    var CubeToSphere = {
        0: function (vin, vout) {
            vout.m_x = vin.m_x;
            vout.m_y = vin.m_y;
            vout.m_z = Math.sqrt(Math.max(0.0, 1.0 - vout.m_x * vout.m_x - vout.m_y * vout.m_y));
        },
        1: function (vin, vout) {
            vout.m_x = -vin.m_x;
            vout.m_y = -vin.m_y;
            vout.m_z = -Math.sqrt(Math.max(0.0, 1.0 - vout.m_x * vout.m_x - vout.m_y * vout.m_y));
        },
        2: function (vin, vout) {
            vout.m_z = vin.m_x;
            vout.m_x = vin.m_y;
            vout.m_y = Math.sqrt(Math.max(0.0, 1.0 - vout.m_x * vout.m_x - vout.m_z * vout.m_z));
        },
        3: function (vin, vout) {
            vout.m_z = -vin.m_x;
            vout.m_x = -vin.m_y;
            vout.m_y = -Math.sqrt(Math.max(0.0, 1.0 - vout.m_x * vout.m_x - vout.m_z * vout.m_z));
        },
        4: function (vin, vout) {
            vout.m_y = vin.m_x;
            vout.m_z = vin.m_y;
            vout.m_x = Math.sqrt(Math.max(0.0, 1.0 - vout.m_y * vout.m_y - vout.m_z * vout.m_z));
        },
        5: function (vin, vout) {
            vout.m_y = -vin.m_x;
            vout.m_z = -vin.m_y;
            vout.m_x = -Math.sqrt(Math.max(0.0, 1.0 - vout.m_y * vout.m_y - vout.m_z * vout.m_z));
        }
    };
    function IntToUInt(value) {
        return (value < 0) ? (-1 - (2 * value)) : (2 * value);
    }
    function UIntToInt(uiValue) {
        return (uiValue & 1) ? -((uiValue + 1) >>> 1) : ((uiValue >>> 1));
    }
    module.Iterator = function () {
        this.m_count = 0;
    }
    module.NumberRef = function () {
        this.m_value = 0;
    }
    // BinaryStream class
    module.BinaryStream = function (buffer) {
        this.m_endianness = SystemEndianness();
        this.m_buffer = buffer;
        this.m_stream = new Uint8Array(this.m_buffer);
        this.m_localBuffer = new ArrayBuffer(4);
        this.m_localBufferViewUChar8 = new Uint8Array(this.m_localBuffer);
        this.m_localBufferViewFloat32 = new Float32Array(this.m_localBuffer);
        this.m_localBufferViewUInt32 = new Uint32Array(this.m_localBuffer);
    }
    module.BinaryStream.prototype.ReadFloat32Bin = function (bsIterator) {
        if (this.m_endianness === O3DGC_BIG_ENDIAN) {
            this.m_localBufferViewUChar8[3] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[2] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[1] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[0] = this.m_stream[bsIterator.m_count++];
        }
        else {
            this.m_localBufferViewUChar8[0] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[1] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[2] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[3] = this.m_stream[bsIterator.m_count++];
        }
        return this.m_localBufferViewFloat32[0];
    };
    module.BinaryStream.prototype.ReadUInt32Bin = function (bsIterator) {
        if (this.m_endianness === O3DGC_BIG_ENDIAN) {
            this.m_localBufferViewUChar8[3] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[2] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[1] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[0] = this.m_stream[bsIterator.m_count++];
        }
        else {
            this.m_localBufferViewUChar8[0] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[1] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[2] = this.m_stream[bsIterator.m_count++];
            this.m_localBufferViewUChar8[3] = this.m_stream[bsIterator.m_count++];
        }
        return this.m_localBufferViewUInt32[0];
    };
    module.BinaryStream.prototype.ReadUChar8Bin = function (bsIterator) {
        return this.m_stream[bsIterator.m_count++];
    };
    module.BinaryStream.prototype.ReadUInt32ASCII = function (bsIterator) {
        var value = 0;
        var shift = 0;
        for (var i = 0; i < O3DGC_BINARY_STREAM_NUM_SYMBOLS_UINT32; ++i) {
            value += (this.m_stream[bsIterator.m_count++] << shift) >>> 0;
            shift += O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0;
        }
        return value;
    };
    module.BinaryStream.prototype.ReadFloat32ASCII = function (bsIterator) {
        var value = this.ReadUInt32ASCII(bsIterator);
        if (this.m_endianness === O3DGC_BIG_ENDIAN) {
            this.m_localBufferViewUChar8[3] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[2] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[1] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[0] = value & O3DGC_MAX_UCHAR8;
        }
        else {
            this.m_localBufferViewUChar8[0] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[1] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[2] = value & O3DGC_MAX_UCHAR8; value >>>= 8;
            this.m_localBufferViewUChar8[3] = value & O3DGC_MAX_UCHAR8;
        }
        return this.m_localBufferViewFloat32[0]; ;
    };
    module.BinaryStream.prototype.ReadIntASCII = function (bsIterator) {
        return UIntToInt(this.ReadUIntASCII(bsIterator));
    };
    module.BinaryStream.prototype.ReadUIntASCII = function (bsIterator) {
        var value = this.m_stream[bsIterator.m_count++];
        if (value === O3DGC_BINARY_STREAM_MAX_SYMBOL0) {
            var i = 0;
            do {
                var x = this.m_stream[bsIterator.m_count++];
                value += ((x >>> 1) << i) >>> 0;
                i += O3DGC_BINARY_STREAM_BITS_PER_SYMBOL1;
            } while (x & 1);
        }
        return value;
    };
    module.BinaryStream.prototype.ReadUCharASCII = function (bsIterator) {
        return this.m_stream[bsIterator.m_count++];
    };
    module.BinaryStream.prototype.ReadFloat32 = function (bsIterator, streamType) {
        if (streamType === O3DGC_STREAM_TYPE_ASCII) {
            return this.ReadFloat32ASCII(bsIterator);
        }
        return this.ReadFloat32Bin(bsIterator);
    };
    module.BinaryStream.prototype.ReadUInt32 = function (bsIterator, streamType) {
        if (streamType === O3DGC_STREAM_TYPE_ASCII) {
            return this.ReadUInt32ASCII(bsIterator);
        }
        return this.ReadUInt32Bin(bsIterator);
    };
    module.BinaryStream.prototype.ReadUChar = function (bsIterator, streamType) {
        if (streamType === O3DGC_STREAM_TYPE_ASCII) {
            return this.ReadUCharASCII(bsIterator);
        }
        return this.ReadUChar8Bin(bsIterator);
    };
    module.BinaryStream.prototype.GetBuffer = function (bsIterator, size) {
        return new Uint8Array(this.m_buffer, bsIterator.m_count, size);
    };

    // Copyright (c) 2004 Amir Said (said@ieee.org) & William A. Pearlman (pearlw@ecse.rpi.edu)
    // All rights reserved.

    var O3DGC_AC_MIN_LENGTH = 0x01000000;   // threshold for renormalization
    var O3DGC_AC_MAX_LENGTH = 0xFFFFFFFF;      // maximum AC interval length
    var O3DGC_AC_BM_LENGTH_SHIFT = 13;     // Maximum values for binary models length bits discarded before mult.
    var O3DGC_AC_BM_MAX_COUNT = (1 << O3DGC_AC_BM_LENGTH_SHIFT) >>> 0;  // for adaptive models
    var O3DGC_AC_DM_LENGTH_SHIFT = 15; // Maximum values for general models length bits discarded before mult.
    var O3DGC_AC_DM_MAX_COUNT = (1 << O3DGC_AC_DM_LENGTH_SHIFT) >>> 0;  // for adaptive models
    // StaticBitModel class 
    module.StaticBitModel = function () {
        this.m_bit0Prob = (1 << (O3DGC_AC_BM_LENGTH_SHIFT - 1)) >>> 0; // p0 = 0.5
    }
    module.StaticBitModel.prototype.SetProbability = function (p) {
        this.m_bit0Prob = Math.floor(p * ((1 << O3DGC_AC_BM_LENGTH_SHIFT) >>> 0));
    }
    // AdaptiveBitModel class 
    module.AdaptiveBitModel = function () {
        // initialization to equiprobable model
        this.m_updateCycle = 4;
        this.m_bitsUntilUpdate = 4;
        this.m_bit0Prob = (1 << (O3DGC_AC_BM_LENGTH_SHIFT - 1)) >>> 0;
        this.m_bit0Count = 1;
        this.m_bitCount = 2;
    }
    module.AdaptiveBitModel.prototype.Reset = function () {
        this.m_updateCycle = 4;
        this.m_bitsUntilUpdate = 4;
        this.m_bit0Prob = (1 << (O3DGC_AC_BM_LENGTH_SHIFT - 1)) >>> 0;
        this.m_bit0Count = 1;
        this.m_bitCount = 2;
    }
    module.AdaptiveBitModel.prototype.Update = function () {
        // halve counts when a threshold is reached
        if ((this.m_bitCount += this.m_updateCycle) > O3DGC_AC_BM_MAX_COUNT) {
            this.m_bitCount = (this.m_bitCount + 1) >>> 1;
            this.m_bit0Count = (this.m_bit0Count + 1) >>> 1;
            if (this.m_bit0Count === this.m_bitCount) {
                ++this.m_bitCount;
            }
        }
        // compute scaled bit 0 probability
        var scale = Math.floor(0x80000000 / this.m_bitCount);
        this.m_bit0Prob = (this.m_bit0Count * scale) >>> (31 - O3DGC_AC_BM_LENGTH_SHIFT);
        // set frequency of model updates
        this.m_updateCycle = (5 * this.m_updateCycle) >>> 2;
        if (this.m_updateCycle > 64) {
            this.m_updateCycle = 64;
        }
        this.m_bitsUntilUpdate = this.m_updateCycle;
    }
    // AdaptiveDataModel class 
    module.AdaptiveDataModel = function () {
        this.m_buffer = {};
        this.m_distribution = {};
        this.m_symbolCount = {};
        this.m_decoderTable = {};
        this.m_totalCount = 0;
        this.m_updateCycle = 0;
        this.m_symbolsUntilUpdate = 0;
        this.m_dataSymbols = 0;
        this.m_lastSymbol = 0;
        this.m_tableSize = 0;
        this.m_tableShift = 0;
    }
    module.AdaptiveDataModel.prototype.Update = function () {
        // halve counts when a threshold is reached
        if ((this.m_totalCount += this.m_updateCycle) > O3DGC_AC_DM_MAX_COUNT) {
            this.m_totalCount = 0;
            for (var n = 0; n < this.m_dataSymbols; ++n) {
                this.m_totalCount += (this.m_symbolCount[n] = (this.m_symbolCount[n] + 1) >>> 1);
            }
        }
        // compute cumulative distribution, decoder table
        var sum = 0;
        var s = 0;
        var scale = Math.floor(0x80000000 / this.m_totalCount);
        if (this.m_tableSize === 0) {
            for (var k = 0; k < this.m_dataSymbols; ++k) {
                this.m_distribution[k] = (scale * sum) >>> (31 - O3DGC_AC_DM_LENGTH_SHIFT);
                sum += this.m_symbolCount[k];
            }
        }
        else {
            for (var k = 0; k < this.m_dataSymbols; ++k) {
                this.m_distribution[k] = (scale * sum) >>> (31 - O3DGC_AC_DM_LENGTH_SHIFT);
                sum += this.m_symbolCount[k];
                var w = this.m_distribution[k] >>> this.m_tableShift;
                while (s < w) {
                    this.m_decoderTable[++s] = k - 1;
                }
            }
            this.m_decoderTable[0] = 0;
            while (s <= this.m_tableSize) {
                this.m_decoderTable[++s] = this.m_dataSymbols - 1;
            }
        }
        // set frequency of model updates
        this.m_updateCycle = (5 * this.m_updateCycle) >>> 2;
        var max_cycle = ((this.m_dataSymbols + 6) << 3) >>> 0;
        if (this.m_updateCycle > max_cycle) {
            this.m_updateCycle = max_cycle;
        }
        this.m_symbolsUntilUpdate = this.m_updateCycle;
    }
    module.AdaptiveDataModel.prototype.Reset = function () {
        if (this.m_dataSymbols === 0) {
            return;
        }
        // restore probability estimates to uniform distribution
        this.m_totalCount = 0;
        this.m_updateCycle = this.m_dataSymbols;
        for (var k = 0; k < this.m_dataSymbols; ++k) {
            this.m_symbolCount[k] = 1;
        }
        this.Update();
        this.m_symbolsUntilUpdate = this.m_updateCycle = (this.m_dataSymbols + 6) >>> 1;
    }
    module.AdaptiveDataModel.prototype.SetAlphabet = function (number_of_symbols) {
        if ((number_of_symbols < 2) || (number_of_symbols > (1 << 11))) {
            Console.log("invalid number of data symbols");
            return O3DGC_ERROR_AC;
        }
        if (this.m_dataSymbols !== number_of_symbols) { // assign memory for data model
            this.m_dataSymbols = number_of_symbols;
            this.m_lastSymbol = this.m_dataSymbols - 1;
            // define size of table for fast decoding
            if (this.m_dataSymbols > 16) {
                var table_bits = 3;
                while (this.m_dataSymbols > ((1 << (table_bits + 2)) >>> 0)) {
                    ++table_bits;
                }
                this.m_tableSize = (1 << table_bits) >>> 0;
                this.m_tableShift = O3DGC_AC_DM_LENGTH_SHIFT - table_bits;
                this.m_buffer = new ArrayBuffer(4 * (2 * this.m_dataSymbols + this.m_tableSize + 2));
                this.m_distribution = new Uint32Array(this.m_buffer, 0, this.m_dataSymbols);
                this.m_symbolCount = new Uint32Array(this.m_buffer, 4 * this.m_dataSymbols, this.m_dataSymbols);
                this.m_decoderTable = new Uint32Array(this.m_buffer, 8 * this.m_dataSymbols, this.m_tableSize + 2);
            }
            else {// small alphabet: no table needed
                this.m_tableSize = this.m_tableShift = 0;
                this.m_buffer = new ArrayBuffer(4 * 2 * this.m_dataSymbols);
                this.m_distribution = new Uint32Array(this.m_buffer, 0, this.m_dataSymbols);
                this.m_symbolCount = new Uint32Array(this.m_buffer, 4 * this.m_dataSymbols, this.m_dataSymbols);
                this.m_decoderTable = {};
            }
        }
        this.Reset(); // initialize model
        return O3DGC_OK;
    }
    // ArithmeticDecoder class
    module.ArithmeticDecoder = function () {
        this.m_codeBuffer = {};
        this.m_acShift = 0;
        this.m_base = 0;
        this.m_value = 0;
        this.m_length = 0; // arithmetic coding state
        this.m_bufferSize = 0;
        this.m_mode = 0; // mode: 0 = undef, 1 = encoder, 2 = decoder
    }
    module.ArithmeticDecoder.prototype.SetBuffer = function (max_code_bytes, user_buffer) {
        if (max_code_bytes === 0) {
            Console.log("invalid codec buffer size");
            return O3DGC_ERROR_AC;
        }
        if (this.m_mode !== 0) {
            Console.log("cannot set buffer while encoding or decoding");
            return O3DGC_ERROR_AC;
        }
        this.m_bufferSize = max_code_bytes;
        this.m_codeBuffer = user_buffer;
    }
    module.ArithmeticDecoder.prototype.StartDecoder = function () {
        if (this.m_mode !== 0) {
            Console.log("cannot start decoder");
            return O3DGC_ERROR_AC;
        }
        if (this.m_bufferSize === 0) {
            Console.log("no code buffer set");
            return O3DGC_ERROR_AC;
        }
        // initialize decoder: interval, pointer, initial code value
        this.m_mode = 2;
        this.m_length = O3DGC_AC_MAX_LENGTH;
        this.m_acShift = 3;
        this.m_value = ((this.m_codeBuffer[0] << 24) | (this.m_codeBuffer[1] << 16) | (this.m_codeBuffer[2] << 8) | (this.m_codeBuffer[3])) >>> 0;
    }
    module.ArithmeticDecoder.prototype.StopDecoder = function () {
        if (this.m_mode !== 2) {
            Console.log("invalid to stop decoder");
            return O3DGC_ERROR_AC;
        }
        this.m_mode = 0;
    }
    module.ArithmeticDecoder.prototype.GetBit = function () {
        this.m_length >>>= 1; // halve interval
        var bit = (this.m_value >= this.m_length); // decode bit
        if (bit) {
            this.m_value -= this.m_length; // move base
        }
        if (this.m_length < O3DGC_AC_MIN_LENGTH) {
            this.RenormDecInterval(); // renormalization
        }
        return bit;
    }
    module.ArithmeticDecoder.prototype.GetBits = function (bits) {
        var s = Math.floor(this.m_value / (this.m_length >>>= bits)); // decode symbol, change length
        this.m_value -= this.m_length * s; // update interval
        if (this.m_length < O3DGC_AC_MIN_LENGTH) {
            this.RenormDecInterval(); // renormalization
        }
        return s;
    }
    module.ArithmeticDecoder.prototype.DecodeStaticBitModel = function (M) {
        var x = M.m_bit0Prob * (this.m_length >>> O3DGC_AC_BM_LENGTH_SHIFT); // product l x p0
        var bit = (this.m_value >= x); // decision
        // update & shift interval
        if (!bit) {
            this.m_length = x;
        }
        else {
            this.m_value -= x; // shifted interval base = 0
            this.m_length -= x;
        }
        if (this.m_length < O3DGC_AC_MIN_LENGTH) {
            this.RenormDecInterval(); // renormalization
        }
        return bit; // return data bit value
    }
    module.ArithmeticDecoder.prototype.DecodeAdaptiveBitModel = function (M) {
        var x = M.m_bit0Prob * (this.m_length >>> O3DGC_AC_BM_LENGTH_SHIFT);   // product l x p0
        var bit = (this.m_value >= x); // decision
        // update interval
        if (!bit) {
            this.m_length = x;
            ++M.m_bit0Count;
        }
        else {
            this.m_value -= x;
            this.m_length -= x;
        }
        if (this.m_length < O3DGC_AC_MIN_LENGTH) {
            this.RenormDecInterval(); // renormalization
        }
        if (--M.m_bitsUntilUpdate === 0) {
            M.Update(); // periodic model update
        }
        return bit; // return data bit value
    }

    module.ArithmeticDecoder.prototype.DecodeAdaptiveDataModel = function (M) {
        var n, s, x;
        var y = this.m_length;
        if (M.m_tableSize > 0) { // use table look-up for faster decoding
            var dv = Math.floor(this.m_value / (this.m_length >>>= O3DGC_AC_DM_LENGTH_SHIFT));
            var t = dv >>> M.m_tableShift;
            s = M.m_decoderTable[t];         // initial decision based on table look-up
            n = M.m_decoderTable[t + 1] + 1;
            while (n > s + 1) { // finish with bisection search
                var m = (s + n) >>> 1;
                if (M.m_distribution[m] > dv) {
                    n = m;
                }
                else {
                    s = m;
                }
            }
            // compute products
            x = M.m_distribution[s] * this.m_length;
            if (s !== M.m_lastSymbol) {
                y = M.m_distribution[s + 1] * this.m_length;
            }
        }
        else { // decode using only multiplications
            x = s = 0;
            this.m_length >>>= O3DGC_AC_DM_LENGTH_SHIFT;
            var m = (n = M.m_dataSymbols) >>> 1;
            // decode via bisection search
            do {
                var z = this.m_length * M.m_distribution[m];
                if (z > this.m_value) {
                    n = m;
                    y = z; // value is smaller
                }
                else {
                    s = m;
                    x = z; // value is larger or equal
                }
            } while ((m = (s + n) >>> 1) !== s);
        }
        this.m_value -= x; // update interval
        this.m_length = y - x;
        if (this.m_length < O3DGC_AC_MIN_LENGTH) {
            this.RenormDecInterval(); // renormalization
        }
        ++M.m_symbolCount[s];
        if (--M.m_symbolsUntilUpdate === 0) {
            M.Update(false); // periodic model update
        }
        return s;
    }
    module.ArithmeticDecoder.prototype.ExpGolombDecode = function (k, bModel0, bModel1) {
        var symbol = 0;
        var binary_symbol = 0;
        do {
            var l = this.DecodeAdaptiveBitModel(bModel1);
            if (l) {
                symbol += (1 << k) >>> 0;
                k++;
            }
        }
        while (l);
        while (k--) { //next binary part
            if (this.DecodeStaticBitModel(bModel0)) {
                binary_symbol = (binary_symbol | (1 << k)) >>> 0;
            }
        }
        return (symbol + binary_symbol);
    }
    module.ArithmeticDecoder.prototype.RenormDecInterval = function () {
        do { // read least-significant byte
            this.m_value = ((this.m_value << 8) | this.m_codeBuffer[++this.m_acShift]) >>> 0;
            this.m_length = (this.m_length << 8) >>> 0;
        } while (this.m_length < O3DGC_AC_MIN_LENGTH); // length multiplied by 256
    }
    module.ArithmeticDecoder.prototype.DecodeIntACEGC = function (mModelValues, bModel0, bModel1, exp_k, M) {
        var uiValue = this.DecodeAdaptiveDataModel(mModelValues);
        if (uiValue === M) {
            uiValue += this.ExpGolombDecode(exp_k, bModel0, bModel1);
        }
        return UIntToInt(uiValue);
    }
    module.ArithmeticDecoder.prototype.DecodeUIntACEGC = function (mModelValues, bModel0, bModel1, exp_k, M) {
        var uiValue = this.DecodeAdaptiveDataModel(mModelValues);
        if (uiValue === M) {
            uiValue += this.ExpGolombDecode(exp_k, bModel0, bModel1);
        }
        return uiValue;
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
    // FIFO class
    module.FIFO = function () {
        this.m_buffer = {};
        this.m_data = {};
        this.m_allocated = 0;
        this.m_size = 0;
        this.m_start = 0;
        this.m_end = 0;
    }
    module.FIFO.prototype.Clear = function () {
        this.m_start = this.m_end = this.m_size = 0;
    }
    module.FIFO.prototype.GetAllocatedSize = function () {
        return this.m_allocated;
    }
    module.FIFO.prototype.GetSize = function () {
        return this.m_size;
    }
    module.FIFO.prototype.Allocate = function (size) {
        if (size > this.m_allocated) {
            this.m_allocated = size;
            this.m_buffer = new ArrayBuffer(4 * m_allocated);
            this.m_data = new Int32Array(this.m_bufferData);
        }
        Clear();
        return O3DGC_OK;
    }
    module.FIFO.prototype.PopFirst = function () {
        --m_size;
        var current = this.m_start++;
        if (this.m_start === this.m_allocated) {
            this.m_end = 0;
        }
        return this.m_data[current];
    }
    module.FIFO.prototype.PushBack = function (value) {
        --m_size;
        this.m_data[this.m_end] = value;
        ++m_size;
        ++m_end;
        if (this.m_end === this.m_allocated) {
            this.m_end = 0;
        }
    }
    // IndexedFaceSet class
    module.IndexedFaceSet = function () {
        this.m_nCoordIndex = 0;
        this.m_nCoord = 0;
        this.m_nNormal = 0;
        this.m_numFloatAttributes = 0;
        this.m_numIntAttributes = 0;
        this.m_creaseAngle = 30.0;
        this.m_ccw = true;
        this.m_solid = true;
        this.m_convex = true;
        this.m_isTriangularMesh = true;
        this.m_buffer = new ArrayBuffer(48 + 12 * O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES + 12 * O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
        var shift = 0;
        this.m_coordMin = new Float32Array(this.m_buffer, shift, 3); shift += 12;
        this.m_coordMax = new Float32Array(this.m_buffer, shift, 3); shift += 12;
        this.m_normalMin = new Float32Array(this.m_buffer, shift, 3); shift += 12;
        this.m_normalMax = new Float32Array(this.m_buffer, shift, 3); shift += 12;
        this.m_nFloatAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES;
        this.m_nIntAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES;
        this.m_dimFloatAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES;
        this.m_dimIntAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES;
        this.m_typeFloatAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES;
        this.m_typeIntAttribute = new Uint32Array(this.m_buffer, shift, O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES); shift += 4 * O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES;
        this.m_minFloatAttributeBuffer = new ArrayBuffer(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        this.m_minFloatAttribute = new Float32Array(this.m_minFloatAttributeBuffer);
        this.m_maxFloatAttributeBuffer = new ArrayBuffer(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        this.m_maxFloatAttribute = new Float32Array(this.m_maxFloatAttributeBuffer);
        this.m_coordIndex = {};
        this.m_coord = {};
        this.m_normal = {};
        this.m_floatAttribute = new Array(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
        this.m_intAttribute = new Array(O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
    }
    module.IndexedFaceSet.prototype.GetNCoordIndex = function () {
        return this.m_nCoordIndex;
    }
    module.IndexedFaceSet.prototype.GetNCoordIndex = function () {
        return this.m_nCoordIndex;
    }
    module.IndexedFaceSet.prototype.GetNCoord = function () {
        return this.m_nCoord;
    }
    module.IndexedFaceSet.prototype.GetNNormal = function () {
        return this.m_nNormal;
    }
    module.IndexedFaceSet.prototype.GetNFloatAttribute = function (a) {
        return this.m_nFloatAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetNIntAttribute = function (a) {
        return this.m_nIntAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetNumFloatAttributes = function () {
        return this.m_numFloatAttributes;
    }
    module.IndexedFaceSet.prototype.GetNumIntAttributes = function () {
        return this.m_numIntAttributes;
    }
    module.IndexedFaceSet.prototype.GetCoordMinArray = function () {
        return this.m_coordMin;
    }
    module.IndexedFaceSet.prototype.GetCoordMaxArray = function () {
        return this.m_coordMax;
    }
    module.IndexedFaceSet.prototype.GetNormalMinArray = function () {
        return this.m_normalMin;
    }
    module.IndexedFaceSet.prototype.GetNormalMaxArray = function () {
        return this.m_normalMax;
    }
    module.IndexedFaceSet.prototype.GetFloatAttributeMinArray = function (a) {
        return (new Float32Array(this.m_minFloatAttributeBuffer, a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES * 4, this.GetFloatAttributeDim(a)));
    }
    module.IndexedFaceSet.prototype.GetFloatAttributeMaxArray = function (a) {
        return (new Float32Array(this.m_maxFloatAttributeBuffer, a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES * 4, this.GetFloatAttributeDim(a)));
    }
    module.IndexedFaceSet.prototype.GetFloatAttributeDim = function (a) {
        return this.m_dimFloatAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetFloatAttributeType = function (a) {
        return this.m_typeFloatAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetIntAttributeType = function (a) {
        return this.m_typeIntAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetIntAttributeDim = function (a) {
        return this.m_minFloatAttribute[a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES + dim];
    }
    module.IndexedFaceSet.prototype.GetFloatAttributeMax = function (a, dim) {
        return this.m_maxFloatAttribute[a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES + dim];
    }
    module.IndexedFaceSet.prototype.GetCreaseAngle = function () {
        return this.m_creaseAngle;
    }
    module.IndexedFaceSet.prototype.GetCreaseAngle = function () {
        return this.m_creaseAngle;
    }
    module.IndexedFaceSet.prototype.GetCCW = function () {
        return this.m_ccw;
    }
    module.IndexedFaceSet.prototype.GetSolid = function () {
        return this.m_solid;
    }
    module.IndexedFaceSet.prototype.GetConvex = function () {
        return this.m_convex;
    }
    module.IndexedFaceSet.prototype.GetIsTriangularMesh = function () {
        return this.m_isTriangularMesh;
    }
    module.IndexedFaceSet.prototype.GetCoordIndex = function () {
        return this.m_coordIndex;
    }
    module.IndexedFaceSet.prototype.GetCoordIndex = function () {
        return this.m_coordIndex;
    }
    module.IndexedFaceSet.prototype.GetCoord = function () {
        return this.m_coord;
    }
    module.IndexedFaceSet.prototype.GetNormal = function () {
        return this.m_normal;
    }
    module.IndexedFaceSet.prototype.GetFloatAttribute = function (a) {
        return this.m_floatAttribute[a];
    }
    module.IndexedFaceSet.prototype.GetIntAttribute = function (a) {
        return this.m_intAttribute[a];
    }
    module.IndexedFaceSet.prototype.SetNCoordIndex = function (nCoordIndex) {
        this.m_nCoordIndex = nCoordIndex;
    }
    module.IndexedFaceSet.prototype.SetNNormalIndex = function (nNormalIndex) {
    }
    module.IndexedFaceSet.prototype.SetNormalPerVertex = function (perVertex) {
    }
    module.IndexedFaceSet.prototype.SetNFloatAttributeIndex = function (nFloatAttributeIndex) {
    }
    module.IndexedFaceSet.prototype.SetNIntAttributeIndex = function (nIntAttributeIndex) {
    }
    module.IndexedFaceSet.prototype.SetFloatAttributePerVertex = function (perVertex) {
    }
    module.IndexedFaceSet.prototype.SetIntAttributePerVertex = function (perVertex) {
    }
    module.IndexedFaceSet.prototype.SetNCoord = function (nCoord) {
        this.m_nCoord = nCoord;
    }
    module.IndexedFaceSet.prototype.SetNNormal = function (nNormal) {
        this.m_nNormal = nNormal;
    }
    module.IndexedFaceSet.prototype.SetNumFloatAttributes = function (numFloatAttributes) {
        this.m_numFloatAttributes = numFloatAttributes;
    }
    module.IndexedFaceSet.prototype.SetNumIntAttributes = function (numIntAttributes) {
        this.m_numIntAttributes = numIntAttributes;
    }
    module.IndexedFaceSet.prototype.SetCreaseAngle = function (creaseAngle) {
        this.m_creaseAngle = creaseAngle;
    }
    module.IndexedFaceSet.prototype.SetCCW = function (ccw) {
        this.m_ccw = ccw;
    }
    module.IndexedFaceSet.prototype.SetSolid = function (solid) {
        this.m_solid = solid;
    }
    module.IndexedFaceSet.prototype.SetConvex = function (convex) {
        this.m_convex = convex;
    }
    module.IndexedFaceSet.prototype.SetIsTriangularMesh = function (isTriangularMesh) {
        this.m_isTriangularMesh = isTriangularMesh;
    }
    module.IndexedFaceSet.prototype.SetCoordMin = function (j, min) {
        this.m_coordMin[j] = min;
    }
    module.IndexedFaceSet.prototype.SetCoordMax = function (j, max) {
        this.m_coordMax[j] = max;
    }
    module.IndexedFaceSet.prototype.SetNormalMin = function (j, min) {
        this.m_normalMin[j] = min;
    }
    module.IndexedFaceSet.prototype.SetNormalMax = function (j, max) {
        this.m_normalMax[j] = max;
    }
    module.IndexedFaceSet.prototype.SetNFloatAttribute = function (a, nFloatAttribute) {
        this.m_nFloatAttribute[a] = nFloatAttribute;
    }
    module.IndexedFaceSet.prototype.SetNIntAttribute = function (a, nIntAttribute) {
        this.m_nIntAttribute[a] = nIntAttribute;
    }
    module.IndexedFaceSet.prototype.SetFloatAttributeDim = function (a, d) {
        this.m_dimFloatAttribute[a] = d;
    }
    module.IndexedFaceSet.prototype.SetIntAttributeDim = function (a, d) {
        this.m_dimIntAttribute[a] = d;
    }
    module.IndexedFaceSet.prototype.SetFloatAttributeType = function (a, d) {
        this.m_typeFloatAttribute[a] = d;
    }
    module.IndexedFaceSet.prototype.SetIntAttributeType = function (a, d) {
        this.m_typeIntAttribute[a] = d;
    }
    module.IndexedFaceSet.prototype.SetFloatAttributeMin = function (a, dim, min) {
        this.m_minFloatAttribute[a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES + dim] = min;
    }
    module.IndexedFaceSet.prototype.SetFloatAttributeMax = function (a, dim, max) {
        this.m_maxFloatAttribute[a * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES + dim] = max;
    }
    module.IndexedFaceSet.prototype.SetCoordIndex = function (coordIndex) {
        this.m_coordIndex = coordIndex;
    }
    module.IndexedFaceSet.prototype.SetCoord = function (coord) {
        this.m_coord = coord;
    }
    module.IndexedFaceSet.prototype.SetNormal = function (normal) {
        this.m_normal = normal;
    }
    module.IndexedFaceSet.prototype.SetFloatAttribute = function (a, floatAttribute) {
        this.m_floatAttribute[a] = floatAttribute;
    }
    module.IndexedFaceSet.prototype.SetIntAttribute = function (a, intAttribute) {
        this.m_intAttribute[a] = intAttribute;
    }

    // SC3DMCEncodeParams class
    module.SC3DMCEncodeParams = function () {
        this.m_numFloatAttributes = 0;
        this.m_numIntAttributes = 0;
        this.m_floatAttributeQuantBits = new Array(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
        this.m_floatAttributePredMode = new Array(O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES);
        this.m_intAttributePredMode = new Array(O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES);
        this.m_encodeMode = O3DGC_SC3DMC_TFAN;
        this.m_streamTypeMode = O3DGC_STREAM_TYPE_ASCII;
        this.m_coordQuantBits = 14;
        this.m_normalQuantBits = 8;
        this.m_coordPredMode = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        this.m_normalPredMode = O3DGC_SC3DMC_SURF_NORMALS_PREDICTION;
        for (var a = 0; a < O3DGC_SC3DMC_MAX_NUM_FLOAT_ATTRIBUTES; ++a) {
            this.m_floatAttributePredMode[a] = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        }
        for (var a = 0; a < O3DGC_SC3DMC_MAX_NUM_INT_ATTRIBUTES; ++a) {
            this.m_intAttributePredMode[a] = O3DGC_SC3DMC_NO_PREDICTION;
        }
    }
    module.SC3DMCEncodeParams.prototype.GetStreamType = function () {
        return this.m_streamTypeMode;
    }
    module.SC3DMCEncodeParams.prototype.GetEncodeMode = function () {
        return this.m_encodeMode;
    }
    module.SC3DMCEncodeParams.prototype.GetNumFloatAttributes = function () {
        return this.m_numFloatAttributes;
    }
    module.SC3DMCEncodeParams.prototype.GetNumIntAttributes = function () {
        return this.m_numIntAttributes;
    }
    module.SC3DMCEncodeParams.prototype.GetCoordQuantBits = function () {
        return this.m_coordQuantBits;
    }
    module.SC3DMCEncodeParams.prototype.GetNormalQuantBits = function () {
        return this.m_normalQuantBits;
    }
    module.SC3DMCEncodeParams.prototype.GetFloatAttributeQuantBits = function (a) {
        return this.m_floatAttributeQuantBits[a];
    }
    module.SC3DMCEncodeParams.prototype.GetCoordPredMode = function () {
        return this.m_coordPredMode;
    }
    module.SC3DMCEncodeParams.prototype.GetNormalPredMode = function () {
        return this.m_normalPredMode;
    }
    module.SC3DMCEncodeParams.prototype.GetFloatAttributePredMode = function (a) {
        return this.m_floatAttributePredMode[a];
    }
    module.SC3DMCEncodeParams.prototype.GetIntAttributePredMode = function (a) {
        return this.m_intAttributePredMode[a];
    }
    module.SC3DMCEncodeParams.prototype.GetCoordPredMode = function () {
        return this.m_coordPredMode;
    }
    module.SC3DMCEncodeParams.prototype.GetNormalPredMode = function () {
        return this.m_normalPredMode;
    }
    module.SC3DMCEncodeParams.prototype.GetFloatAttributePredMode = function (a) {
        return this.m_floatAttributePredMode[a];
    }
    module.SC3DMCEncodeParams.prototype.GetIntAttributePredMode = function (a) {
        return this.m_intAttributePredMode[a];
    }
    module.SC3DMCEncodeParams.prototype.SetStreamType = function (streamTypeMode) {
        this.m_streamTypeMode = streamTypeMode;
    }
    module.SC3DMCEncodeParams.prototype.SetEncodeMode = function (encodeMode) {
        this.m_encodeMode = encodeMode;
    }
    module.SC3DMCEncodeParams.prototype.SetNumFloatAttributes = function (numFloatAttributes) {
        this.m_numFloatAttributes = numFloatAttributes;
    }
    module.SC3DMCEncodeParams.prototype.SetNumIntAttributes = function (numIntAttributes) {
        this.m_numIntAttributes = numIntAttributes;
    }
    module.SC3DMCEncodeParams.prototype.SetCoordQuantBits = function (coordQuantBits) {
        this.m_coordQuantBits = coordQuantBits;
    }
    module.SC3DMCEncodeParams.prototype.SetNormalQuantBits = function (normalQuantBits) {
        this.m_normalQuantBits = normalQuantBits;
    }
    module.SC3DMCEncodeParams.prototype.SetFloatAttributeQuantBits = function (a, q) {
        this.m_floatAttributeQuantBits[a] = q;
    }
    module.SC3DMCEncodeParams.prototype.SetCoordPredMode = function (coordPredMode) {
        this.m_coordPredMode = coordPredMode;
    }
    module.SC3DMCEncodeParams.prototype.SetNormalPredMode = function (normalPredMode) {
        this.m_normalPredMode = normalPredMode;
    }
    module.SC3DMCEncodeParams.prototype.SetFloatAttributePredMode = function (a, p) {
        this.m_floatAttributePredMode[a] = p;
    }
    module.SC3DMCEncodeParams.prototype.SetIntAttributePredMode = function (a, p) {
        this.m_intAttributePredMode[a] = p;
    }
    // AdjacencyInfo class
    module.AdjacencyInfo = function () {
        this.m_neighborsSize = 0;    // actual allocated size for m_neighbors
        this.m_numNeighborsSize = 0; // actual allocated size for m_numNeighbors
        this.m_numElements = 0;      // number of elements 
        this.m_neighborsBuffer = {};
        this.m_numNeighborsBuffer = {};
        this.m_neighbors = {};
        this.m_numNeighbors = {};
    }
    module.AdjacencyInfo.prototype.Allocate = function (numNeighborsSize, neighborsSize) {
        this.m_numElements = numNeighborsSize;
        if (neighborsSize > this.m_neighborsSize) {
            this.m_neighborsSize = neighborsSize;
            this.m_neighborsBuffer = new ArrayBuffer(4 * this.m_neighborsSize);
            this.m_neighbors = new Int32Array(this.m_neighborsBuffer);
        }
        if (numNeighborsSize > this.m_numNeighborsSize) {
            this.m_numNeighborsSize = numNeighborsSize;
            this.m_numNeighborsBuffer = new ArrayBuffer(4 * this.m_numNeighborsSize);
            this.m_numNeighbors = new Int32Array(this.m_numNeighborsBuffer);
        }
        return O3DGC_OK;
    }
    module.AdjacencyInfo.prototype.AllocateNumNeighborsArray = function (numElements) {
        if (numElements > this.m_numNeighborsSize) {
            this.m_numNeighborsSize = numElements;
            this.m_numNeighborsBuffer = new ArrayBuffer(4 * this.m_numNeighborsSize);
            this.m_numNeighbors = new Int32Array(this.m_numNeighborsBuffer);
        }
        this.m_numElements = numElements;
        return O3DGC_OK;
    }
    module.AdjacencyInfo.prototype.AllocateNeighborsArray = function () {
        for (var i = 1; i < this.m_numElements; ++i) {
            this.m_numNeighbors[i] += this.m_numNeighbors[i - 1];
        }
        if (this.m_numNeighbors[this.m_numElements - 1] > this.m_neighborsSize) {
            this.m_neighborsSize = this.m_numNeighbors[this.m_numElements - 1];
            this.m_neighborsBuffer = new ArrayBuffer(4 * this.m_neighborsSize);
            this.m_neighbors = new Int32Array(this.m_neighborsBuffer);
        }
        return O3DGC_OK;
    }
    module.AdjacencyInfo.prototype.ClearNumNeighborsArray = function () {
        for (var i = 0; i < this.m_numElements; ++i) {
            this.m_numNeighbors[i] = 0;
        }
        return O3DGC_OK;
    }
    module.AdjacencyInfo.prototype.ClearNeighborsArray = function () {
        for (var i = 0; i < this.m_neighborsSize; ++i) {
            this.m_neighbors[i] = -1;
        }
        return O3DGC_OK;
    }
    module.AdjacencyInfo.prototype.Begin = function (element) {
        return (element > 0) ? this.m_numNeighbors[element - 1] : 0;
    }
    module.AdjacencyInfo.prototype.End = function (element) {
        return this.m_numNeighbors[element];
    }
    module.AdjacencyInfo.prototype.AddNeighbor = function (element, neighbor) {
        var p0 = this.Begin(element);
        var p1 = this.End(element);
        for (var p = p0; p < p1; ++p) {
            if (this.m_neighbors[p] === -1) {
                this.m_neighbors[p] = neighbor;
                return O3DGC_OK;
            }
        }
        return O3DGC_ERROR_BUFFER_FULL;
    }
    module.AdjacencyInfo.prototype.GetNeighbor = function (element) {
        return this.m_neighbors[element];
    }
    module.AdjacencyInfo.prototype.GetNumNeighbors = function (element) {
        return this.End(element) - this.Begin(element);
    }
    module.AdjacencyInfo.prototype.GetNumNeighborsBuffer = function () {
        return this.m_numNeighbors;
    }
    module.AdjacencyInfo.prototype.GetNeighborsBuffer = function () {
        return this.m_neighbors;
    }
    // Vector class
    module.Vector = function () {
        this.m_buffer = {};
        this.m_data = {};
        this.m_allocated = 0;
        this.m_size = 0;
    }
    module.Vector.prototype.Clear = function () {
        this.m_size = 0;
    }
    module.Vector.prototype.Get = function (i) {
        return this.m_data[i];
    }
    module.Vector.prototype.GetAllocatedSize = function () {
        return this.m_allocated;
    }
    module.Vector.prototype.GetSize = function () {
        return this.m_size;
    }
    module.Vector.prototype.GetBuffer = function () {
        return this.m_data;
    }
    module.Vector.prototype.SetSize = function (size) {
        this.m_size = size;
    }
    module.Vector.prototype.Allocate = function (size) {
        if (size > this.m_allocated) {
            this.m_allocated = size;
            var tmp_buffer = new ArrayBuffer(4 * this.m_allocated);
            var tmp_data = new Int32Array(tmp_buffer);
            if (this.m_size > 0) {
                for (var i = 0; i < this.m_size; ++i) {
                    tmp_data[i] = this.m_data[i];
                }
            }
            this.m_buffer = tmp_buffer;
            this.m_data = tmp_data;
        }
    }
    module.Vector.prototype.PushBack = function (value) {
        if (this.m_size === this.m_allocated) {
            this.m_allocated *= 2;
            if (this.m_allocated < O3DGC_DEFAULT_VECTOR_SIZE) {
                this.m_allocated = O3DGC_DEFAULT_VECTOR_SIZE;
            }
            var tmp_buffer = new ArrayBuffer(4 * this.m_allocated);
            var tmp_data = new Int32Array(tmp_buffer);
            if (this.m_size > 0) {
                for (var i = 0; i < this.m_size; ++i) {
                    tmp_data[i] = this.m_data[i];
                }
            }
            this.m_buffer = tmp_buffer;
            this.m_data = tmp_data;
        }
        this.m_data[this.m_size++] = value;
    }
    // CompressedTriangleFans class
    module.CompressedTriangleFans = function () {
        this.m_numTFANs = new module.Vector();
        this.m_degrees = new module.Vector();
        this.m_configs = new module.Vector();
        this.m_operations = new module.Vector();
        this.m_indices = new module.Vector();
        this.m_trianglesOrder = new module.Vector();
        this.m_streamType = O3DGC_STREAM_TYPE_UNKOWN;
    }
    module.CompressedTriangleFans.prototype.GetStreamType = function () {
        return this.m_streamType;
    }
    module.CompressedTriangleFans.prototype.SetStreamType = function (streamType) {
        this.m_streamType = streamType;
    }
    module.CompressedTriangleFans.prototype.Clear = function () {
        this.m_numTFANs.Clear();
        this.m_degrees.Clear();
        this.m_configs.Clear();
        this.m_operations.Clear();
        this.m_indices.Clear();
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.Allocate = function (numVertices, numTriangles) {
        this.m_numTFANs.Allocate(numVertices);
        this.m_degrees.Allocate(2 * numVertices);
        this.m_configs.Allocate(2 * numVertices);
        this.m_operations.Allocate(2 * numVertices);
        this.m_indices.Allocate(2 * numVertices);
        this.m_trianglesOrder.Allocate(numTriangles);
        this.Clear();
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.PushNumTFans = function (numTFans) {
        this.m_numTFANs.PushBack(numTFans);
    }
    module.CompressedTriangleFans.prototype.ReadNumTFans = function (it) {
        return this.m_numTFANs.Get(it.m_count++);
    }
    module.CompressedTriangleFans.prototype.PushDegree = function (degree) {
        this.m_degrees.PushBack(degree);
    }
    module.CompressedTriangleFans.prototype.ReadDegree = function (it) {
        return this.m_degrees.Get(it.m_count++);
    }
    module.CompressedTriangleFans.prototype.PushConfig = function (config) {
        this.m_configs.PushBack(config);
    }
    module.CompressedTriangleFans.prototype.ReadConfig = function (it) {
        return this.m_configs.Get(it.m_count++);
    }
    module.CompressedTriangleFans.prototype.PushOperation = function (op) {
        this.m_operations.PushBack(op);
    }
    module.CompressedTriangleFans.prototype.ReadOperation = function (it) {
        return this.m_operations.Get(it.m_count++);
    }
    module.CompressedTriangleFans.prototype.PushIndex = function (index) {
        this.m_indices.PushBack(index);
    }
    module.CompressedTriangleFans.prototype.ReadIndex = function (it) {
        return this.m_indices.Get(it.m_count++);
    }
    module.CompressedTriangleFans.prototype.PushTriangleIndex = function (index) {
        this.m_trianglesOrder.PushBack(IntToUInt(index));
    }
    module.CompressedTriangleFans.prototype.ReadTriangleIndex = function (it) {
        return UIntToInt(this.m_trianglesOrder.Get(it.m_count++));
    }
    module.CompressedTriangleFans.prototype.LoadUIntData = function (data, bstream, it) {
        bstream.ReadUInt32ASCII(it);
        var size = bstream.ReadUInt32ASCII(it);
        data.Allocate(size);
        data.Clear();
        for (var i = 0; i < size; ++i) {
            data.PushBack(bstream.ReadUIntASCII(it));
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.LoadIntData = function (data, bstream, it) {
        bstream.ReadUInt32ASCII(it);
        var size = bstream.ReadUInt32ASCII(it);
        data.Allocate(size);
        data.Clear();
        for (var i = 0; i < size; ++i) {
            data.PushBack(bstream.ReadIntASCII(it));
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.LoadBinData = function (data, bstream, it) {
        bstream.ReadUInt32ASCII(it);
        var size = bstream.ReadUInt32ASCII(it);
        var symbol;
        data.Allocate(size * O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0);
        data.Clear();
        for (var i = 0; i < size; ) {
            symbol = bstream.ReadUCharASCII(it);
            for (var h = 0; h < O3DGC_BINARY_STREAM_BITS_PER_SYMBOL0; ++h) {
                data.PushBack(symbol & 1);
                symbol >>>= 1;
                ++i;
            }
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.LoadUIntAC = function (data, M, bstream, it) {
        var sizeSize = bstream.ReadUInt32Bin(it) - 12;
        var size = bstream.ReadUInt32Bin(it);
        if (size === 0) {
            return O3DGC_OK;
        }
        var minValue = bstream.ReadUInt32Bin(it);
        var buffer = bstream.GetBuffer(it, sizeSize);
        it.m_count += sizeSize;
        data.Allocate(size);
        var acd = new module.ArithmeticDecoder();
        acd.SetBuffer(sizeSize, buffer);
        acd.StartDecoder();
        var mModelValues = new module.AdaptiveDataModel();
        mModelValues.SetAlphabet(M + 1);
        for (var i = 0; i < size; ++i) {
            data.PushBack(acd.DecodeAdaptiveDataModel(mModelValues) + minValue);
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.LoadIntACEGC = function (data, M, bstream, it) {
        var sizeSize = bstream.ReadUInt32Bin(it) - 12;
        var size = bstream.ReadUInt32Bin(it);
        if (size === 0) {
            return O3DGC_OK;
        }
        var minValue = bstream.ReadUInt32Bin(it) - O3DGC_MAX_LONG;
        var buffer = bstream.GetBuffer(it, sizeSize);
        it.m_count += sizeSize;
        data.Allocate(size);
        var acd = new module.ArithmeticDecoder();
        acd.SetBuffer(sizeSize, buffer);
        acd.StartDecoder();
        var mModelValues = new module.AdaptiveDataModel();
        mModelValues.SetAlphabet(M + 2);
        var bModel0 = new module.StaticBitModel();
        var bModel1 = new module.AdaptiveBitModel();
        var value;
        for (var i = 0; i < size; ++i) {
            value = acd.DecodeAdaptiveDataModel(mModelValues);
            if (value === M) {
                value += acd.ExpGolombDecode(0, bModel0, bModel1);
            }
            data.PushBack(value + minValue);
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.LoadBinAC = function (data, bstream, it) {
        var sizeSize = bstream.ReadUInt32Bin(it) - 8;
        var size = bstream.ReadUInt32Bin(it);
        if (size === 0) {
            return O3DGC_OK;
        }
        var buffer = bstream.GetBuffer(it, sizeSize);
        it.m_count += sizeSize;
        data.Allocate(size);
        var acd = new module.ArithmeticDecoder();
        acd.SetBuffer(sizeSize, buffer);
        acd.StartDecoder();
        var bModel = new module.AdaptiveBitModel();
        for (var i = 0; i < size; ++i) {
            data.PushBack(acd.DecodeAdaptiveBitModel(bModel));
        }
        return O3DGC_OK;
    }
    module.CompressedTriangleFans.prototype.Load = function (bstream, iterator, decodeTrianglesOrder, streamType) {
        if (streamType === O3DGC_STREAM_TYPE_ASCII) {
            this.LoadUIntData(this.m_numTFANs, bstream, iterator);
            this.LoadUIntData(this.m_degrees, bstream, iterator);
            this.LoadUIntData(this.m_configs, bstream, iterator);
            this.LoadBinData(this.m_operations, bstream, iterator);
            this.LoadIntData(this.m_indices, bstream, iterator);
            if (decodeTrianglesOrder) {
                this.LoadUIntData(this.m_trianglesOrder, bstream, iterator);
            }
        }
        else {
            this.LoadIntACEGC(this.m_numTFANs, 4, bstream, iterator);
            this.LoadIntACEGC(this.m_degrees, 16, bstream, iterator);
            this.LoadUIntAC(this.m_configs, 10, bstream, iterator);
            this.LoadBinAC(this.m_operations, bstream, iterator);
            this.LoadIntACEGC(this.m_indices, 8, bstream, iterator);
            if (decodeTrianglesOrder) {
                this.LoadIntACEGC(this.m_trianglesOrder, 16, bstream, iterator);
            }
        }
        return O3DGC_OK;
    }
    // TriangleFans class
    module.TriangleFans = function () {
        this.m_verticesAllocatedSize = 0;
        this.m_sizeTFANAllocatedSize = 0;
        this.m_numTFANs = 0;
        this.m_numVertices = 0;
        this.m_sizeTFANBuffer = {};
        this.m_sizeTFAN = {};
        this.m_verticesBuffer = {};
        this.m_vertices = {};
    }
    module.TriangleFans.prototype.Allocate = function (sizeTFAN, verticesSize) {
        this.m_numTFANs = 0;
        this.m_numVertices = 0;
        if (this.m_verticesAllocatedSize < verticesSize) {
            this.m_verticesAllocatedSize = verticesSize;
            this.m_verticesBuffer = new ArrayBuffer(4 * this.m_verticesAllocatedSize);
            this.m_vertices = new Int32Array(this.m_verticesBuffer);
        }
        if (this.m_sizeTFANAllocatedSize < sizeTFAN) {
            this.m_sizeTFANAllocatedSize = sizeTFAN;
            this.m_sizeTFANBuffer = new ArrayBuffer(4 * this.m_sizeTFANAllocatedSize);
            this.m_sizeTFAN = new Int32Array(this.m_verticesBuffer);
        }
        return O3DGC_OK;
    }
    module.TriangleFans.prototype.Clear = function () {
        this.m_numTFANs = 0;
        this.m_numVertices = 0;
        return O3DGC_OK;
    }
    module.TriangleFans.prototype.AddVertex = function (vertex) {
        ++this.m_numVertices;
        if (this.m_numVertices > this.m_verticesAllocatedSize) {
            this.m_verticesAllocatedSize *= 2;
            var tmp_verticesBuffer = new ArrayBuffer(4 * this.m_verticesAllocatedSize);
            var tmp_vertices = new Int32Array(tmp_verticesBuffer);
            for (var i = 0; i < this.m_numVertices; ++i) {
                tmp_vertices[i] = this.m_vertices[i];
            }
            this.m_buffer = tmp_verticesBuffer;
            this.m_vertices = tmp_vertices;
        }
        this.m_vertices[this.m_numVertices - 1] = vertex;
        ++this.m_sizeTFAN[this.m_numTFANs - 1];
        return O3DGC_OK;
    }
    module.TriangleFans.prototype.AddTFAN = function (vertex) {
        ++this.m_numTFANs;
        if (this.m_numTFANs > this.m_sizeTFANAllocatedSize) {
            this.m_sizeTFANAllocatedSize *= 2;
            var tmp_sizeTFANBuffer = new ArrayBuffer(4 * this.m_sizeTFANAllocatedSize);
            var tmp_sizeTFAN = new Int32Array(tmp_sizeTFANBuffer);
            for (var i = 0; i < this.m_numTFANs; ++i) {
                tmp_sizeTFAN[i] = this.m_sizeTFAN[i];
            }
            this.m_sizeTFANBuffer = tmp_sizeTFANBuffer;
            this.m_sizeTFAN = tmp_sizeTFAN;
        }
        this.m_sizeTFAN[this.m_numTFANs - 1] = (this.m_numTFANs > 1) ? this.m_sizeTFAN[this.m_numTFANs - 2] : 0;
        return O3DGC_OK;
    }
    module.TriangleFans.prototype.Begin = function (tfan) {
        return (tfan > 0) ? this.m_sizeTFAN[tfan - 1] : 0;
    }
    module.TriangleFans.prototype.End = function (tfan) {
        return this.m_sizeTFAN[tfan];
    }
    module.TriangleFans.prototype.GetVertex = function (vertex) {
        return this.m_vertices[vertex];
    }
    module.TriangleFans.prototype.GetTFANSize = function (tfan) {
        return End(tfan) - Begin(tfan);
    }
    module.TriangleFans.prototype.GetNumTFANs = function () {
        return this.m_numTFANs;
    }
    module.TriangleFans.prototype.GetNumVertices = function () {
        return this.m_numVertices;
    }
    // TriangleListDecoder class
    module.TriangleListDecoder = function (sizeTFAN, verticesSize) {
        this.m_itNumTFans = new module.Iterator();
        this.m_itDegree = new module.Iterator();
        this.m_itConfig = new module.Iterator();
        this.m_itOperation = new module.Iterator();
        this.m_itIndex = new module.Iterator();
        this.m_maxNumVertices = 0;
        this.m_maxNumTriangles = 0;
        this.m_numTriangles = 0;
        this.m_numVertices = 0;
        this.m_tempTrianglesSize = 0;
        this.m_vertexCount = 0;
        this.m_triangleCount = 0;
        this.m_numConqueredTriangles = 0;
        this.m_numVisitedVertices = 0;
        this.m_triangles = {};
        this.m_visitedVerticesBuffer = {}
        this.m_visitedVertices = {};
        this.m_visitedVerticesValence = {};
        this.m_vertexToTriangle = new module.AdjacencyInfo();
        this.m_ctfans = new module.CompressedTriangleFans();
        this.m_tfans = new module.TriangleFans();
        this.m_streamType = O3DGC_STREAM_TYPE_ASCII;
        this.m_decodeTrianglesOrder = false;
        this.m_decodeVerticesOrder = false;
        this.m_processConfig = {
            0: function (decoder, degree) { // ops: 1000001 vertices: -1 -2
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
                for (var u = 1; u < degree - 1; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
            },
            1: function (decoder, degree, focusVertex) { // ops: 1xxxxxx1 vertices: -1 x x x x x -2
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
                for (var u = 1; u < degree - 1; ++u) {
                    var op = decoder.m_ctfans.ReadOperation(decoder.m_itOperation);
                    if (op === 1) {
                        var index = decoder.m_ctfans.ReadIndex(decoder.m_itIndex);
                        if (index < 0) {
                            decoder.m_tfans.AddVertex(decoder.m_visitedVertices[-index - 1]);
                        }
                        else {
                            decoder.m_tfans.AddVertex(index + focusVertex);
                        }
                    }
                    else {
                        decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                        decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                    }
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
            },
            2: function (decoder, degree) { // ops: 00000001 vertices: -1
                for (var u = 0; u < degree - 1; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
            },
            3: function (decoder, degree) { // ops: 00000001 vertices: -2
                for (var u = 0; u < degree - 1; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
            },
            4: function (decoder, degree) {// ops: 10000000 vertices: -1
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
                for (var u = 1; u < degree; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
            },
            5: function (decoder, degree) { // ops: 10000000 vertices: -2
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
                for (var u = 1; u < degree; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
            },
            6: function (decoder, degree) { // ops: 00000000 vertices:
                for (var u = 0; u < degree; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
            },
            7: function (decoder, degree) { // ops: 1000001 vertices: -2 -1
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
                for (var u = 1; u < degree - 1; ++u) {
                    decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                    decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
            },
            8: function (decoder, degree, focusVertex) { // ops: 1xxxxxx1 vertices: -2 x x x x x -1
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[1]);
                for (var u = 1; u < degree - 1; ++u) {
                    var op = decoder.m_ctfans.ReadOperation(decoder.m_itOperation);
                    if (op === 1) {
                        var index = decoder.m_ctfans.ReadIndex(decoder.m_itIndex);
                        if (index < 0) {
                            decoder.m_tfans.AddVertex(decoder.m_visitedVertices[-index - 1]);
                        }
                        else {
                            decoder.m_tfans.AddVertex(index + focusVertex);
                        }
                    }
                    else {
                        decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                        decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                    }
                }
                decoder.m_tfans.AddVertex(decoder.m_visitedVertices[0]);
            },
            9: function (decoder, degree, focusVertex) { // general case
                for (var u = 0; u < degree; ++u) {
                    var op = decoder.m_ctfans.ReadOperation(decoder.m_itOperation);
                    if (op === 1) {
                        var index = decoder.m_ctfans.ReadIndex(decoder.m_itIndex);
                        if (index < 0) {
                            decoder.m_tfans.AddVertex(decoder.m_visitedVertices[-index - 1]);
                        }
                        else {
                            decoder.m_tfans.AddVertex(index + focusVertex);
                        }
                    }
                    else {
                        decoder.m_visitedVertices[decoder.m_numVisitedVertices++] = decoder.m_vertexCount;
                        decoder.m_tfans.AddVertex(decoder.m_vertexCount++);
                    }
                }
            }
        };
    }
    module.TriangleListDecoder.prototype.GetStreamType = function () {
        return this.m_streamType;
    }
    module.TriangleListDecoder.prototype.GetReorderTriangles = function () {
        return this.m_decodeTrianglesOrder;
    }
    module.TriangleListDecoder.prototype.GetReorderVertices = function () {
        return this.m_decodeVerticesOrder;
    }
    module.TriangleListDecoder.prototype.SetStreamType = function (streamType) {
        this.m_streamType = streamType;
    }
    module.TriangleListDecoder.prototype.GetVertexToTriangle = function () {
        return this.m_vertexToTriangle;
    }
    module.TriangleListDecoder.prototype.Reorder = function () {
        if (this.m_decodeTrianglesOrder) {
            var _triangles = this.m_triangles;
            var itTriangleIndex = 0;
            var prevTriangleIndex = 0;
            var numIndices = this.m_numTriangles * 3;
            var tempTriangles = new Int32Array(_triangles);
            for (var i = 0; i < this.m_numTriangles; ++i) {
                var t = this.m_ctfans.ReadTriangleIndex(itTriangleIndex) + prevTriangleIndex;
                _triangles[3 * t] = tempTriangles[3 * i];
                _triangles[3 * t + 1] = tempTriangles[3 * i + 1];
                _triangles[3 * t + 2] = tempTriangles[3 * i + 2];
                prevTriangleIndex = t + 1;
            }
        }
        return O3DGC_OK;
    }
    module.TriangleListDecoder.prototype.CompueLocalConnectivityInfo = function (focusVertex) {
        var t = 0;
        var p, v;
        this.m_numConqueredTriangles = 0;
        this.m_numVisitedVertices = 0;
        for (var i = this.m_vertexToTriangle.Begin(focusVertex); (t >= 0) && (i < this.m_vertexToTriangle.End(focusVertex)); ++i) {
            t = this.m_vertexToTriangle.GetNeighbor(i);
            if (t >= 0) {
                ++this.m_numConqueredTriangles;
                p = 3 * t;
                // extract visited vertices
                for (var k = 0; k < 3; ++k) {
                    v = this.m_triangles[p + k];
                    if (v > focusVertex) { // vertices are insertices by increasing traversal order
                        var foundOrInserted = false;
                        for (var j = 0; j < this.m_numVisitedVertices; ++j) {
                            if (v === this.m_visitedVertices[j]) {
                                this.m_visitedVerticesValence[j]++;
                                foundOrInserted = true;
                                break;
                            }
                            else if (v < this.m_visitedVertices[j]) {
                                ++this.m_numVisitedVertices;
                                for (var h = this.m_numVisitedVertices - 1; h > j; --h) {
                                    this.m_visitedVertices[h] = this.m_visitedVertices[h - 1];
                                    this.m_visitedVerticesValence[h] = this.m_visitedVerticesValence[h - 1];
                                }
                                this.m_visitedVertices[j] = v;
                                this.m_visitedVerticesValence[j] = 1;
                                foundOrInserted = true;
                                break;
                            }
                        }
                        if (!foundOrInserted) {
                            this.m_visitedVertices[this.m_numVisitedVertices] = v;
                            this.m_visitedVerticesValence[this.m_numVisitedVertices] = 1;
                            this.m_numVisitedVertices++;
                        }
                    }
                }
            }
        }
        // re-order visited vertices by taking into account their valence (i.e., # of conquered triangles incident to each vertex)
        // in order to avoid config. 9
        if (this.m_numVisitedVertices > 2) {
            var y;
            for (var x = 1; x < this.m_numVisitedVertices; ++x) {
                if (this.m_visitedVerticesValence[x] === 1) {
                    y = x;
                    while ((y > 0) && (this.m_visitedVerticesValence[y] < this.m_visitedVerticesValence[y - 1])) {
                        var tmp = this.m_visitedVerticesValence[y];
                        this.m_visitedVerticesValence[y] = this.m_visitedVerticesValence[y - 1];
                        this.m_visitedVerticesValence[y - 1] = tmp;
                        tmp = this.m_visitedVertices[y];
                        this.m_visitedVertices[y] = this.m_visitedVertices[y - 1];
                        this.m_visitedVertices[y - 1] = tmp;
                        --y;
                    }
                }
            }
        }
        return O3DGC_OK;
    }
    module.TriangleListDecoder.prototype.DecompressTFAN = function (focusVertex) {
        var degree, config;
        var op;
        var index;
        var k0, k1;
        var b, c, t;
        var ntfans = this.m_ctfans.ReadNumTFans(this.m_itNumTFans);
        if (ntfans > 0) {
            for (var f = 0; f != ntfans; ++f) {
                this.m_tfans.AddTFAN();
                degree = this.m_ctfans.ReadDegree(this.m_itDegree) + 2 - this.m_numConqueredTriangles;
                config = this.m_ctfans.ReadConfig(this.m_itConfig);
                k0 = this.m_tfans.GetNumVertices();
                this.m_tfans.AddVertex(focusVertex);
                this.m_processConfig[config](this, degree, focusVertex);
                k1 = this.m_tfans.GetNumVertices();
                b = this.m_tfans.GetVertex(k0 + 1);
                for (var k = k0 + 2; k < k1; ++k) {
                    c = this.m_tfans.GetVertex(k);
                    t = this.m_triangleCount * 3;
                    this.m_triangles[t++] = focusVertex;
                    this.m_triangles[t++] = b;
                    this.m_triangles[t] = c;

                    this.m_vertexToTriangle.AddNeighbor(focusVertex, this.m_triangleCount);
                    this.m_vertexToTriangle.AddNeighbor(b, this.m_triangleCount);
                    this.m_vertexToTriangle.AddNeighbor(c, this.m_triangleCount);
                    b = c;
                    this.m_triangleCount++;
                }
            }
        }
        return O3DGC_OK;
    }
    module.TriangleListDecoder.prototype.Decompress = function () {
        for (var focusVertex = 0; focusVertex < this.m_numVertices; ++focusVertex) {
            if (focusVertex === this.m_vertexCount) {
                this.m_vertexCount++; // insert focusVertex
            }
            this.CompueLocalConnectivityInfo(focusVertex);
            this.DecompressTFAN(focusVertex);
        }
        return O3DGC_OK;
    }
    module.TriangleListDecoder.prototype.Init = function (triangles, numTriangles, numVertices, maxSizeV2T) {
        this.m_numTriangles = numTriangles;
        this.m_numVertices = numVertices;
        this.m_triangles = triangles;
        this.m_vertexCount = 0;
        this.m_triangleCount = 0;
        this.m_itNumTFans.m_count = 0;
        this.m_itDegree.m_count = 0;
        this.m_itConfig.m_count = 0;
        this.m_itOperation.m_count = 0;
        this.m_itIndex.m_count = 0;
        if (this.m_numVertices > this.m_maxNumVertices) {
            this.m_maxNumVertices = this.m_numVertices;
            this.m_visitedVerticesBuffer = new ArrayBuffer(8 * this.m_numVertices);
            this.m_visitedVerticesValence = new Int32Array(this.m_visitedVerticesBuffer, 0, this.m_numVertices);
            this.m_visitedVertices = new Int32Array(this.m_visitedVerticesBuffer, 4 * this.m_numVertices, this.m_numVertices);
        }
        if (this.m_decodeTrianglesOrder && this.m_tempTrianglesSize < this.m_numTriangles) {
            this.m_tempTrianglesSize = this.m_numTriangles;
            this.m_tempTrianglesBuffer = new ArrayBuffer(12 * this.m_tempTrianglesSize);
            this.m_tempTriangles = new Int32Array(this.m_tempTrianglesBuffer);
        }
        this.m_ctfans.SetStreamType(this.m_streamType);
        this.m_ctfans.Allocate(this.m_numVertices, this.m_numTriangles);
        this.m_tfans.Allocate(2 * this.m_numVertices, 8 * this.m_numVertices);
        // compute vertex-to-triangle adjacency information
        this.m_vertexToTriangle.AllocateNumNeighborsArray(numVertices);
        var numNeighbors = this.m_vertexToTriangle.GetNumNeighborsBuffer();
        for (var i = 0; i < numVertices; ++i) {
            numNeighbors[i] = maxSizeV2T;
        }
        this.m_vertexToTriangle.AllocateNeighborsArray();
        this.m_vertexToTriangle.ClearNeighborsArray();
        return O3DGC_OK;
    }
    module.TriangleListDecoder.prototype.Decode = function (triangles, numTriangles, numVertices, bstream, it) {
        var compressionMask = bstream.ReadUChar(it, this.m_streamType);
        this.m_decodeTrianglesOrder = ((compressionMask & 2) !== 0);
        this.m_decodeVerticesOrder = ((compressionMask & 1) !== 0);
        if (this.m_decodeVerticesOrder) { // vertices reordering not supported
            return O3DGC_ERROR_NON_SUPPORTED_FEATURE;
        }
        var maxSizeV2T = bstream.ReadUInt32(it, this.m_streamType);
        this.Init(triangles, numTriangles, numVertices, maxSizeV2T);
        this.m_ctfans.Load(bstream, it, this.m_decodeTrianglesOrder, this.m_streamType);
        this.Decompress();
        return O3DGC_OK;
    }
    // SC3DMCDecoder class
    module.SC3DMCDecoder = function () {
        this.m_iterator = new module.Iterator();
        this.m_streamSize = 0;
        this.m_params = new module.SC3DMCEncodeParams();
        this.m_triangleListDecoder = new module.TriangleListDecoder();
        this.m_quantFloatArrayBuffer = {};
        this.m_orientationBuffer = {};
        this.m_normalsBuffer = {};
        this.m_quantFloatArray = {};
        this.m_orientation = {};
        this.m_normals = {};
        this.m_quantFloatArraySize = 0;
        this.m_normalsSize = 0;
        this.m_orientationSize = 0;
        this.m_stats = new module.SC3DMCStats();
        this.m_streamType = O3DGC_STREAM_TYPE_UNKOWN;
        this.m_neighbors = new Array(O3DGC_SC3DMC_MAX_PREDICTION_NEIGHBORS);
        this.m_ideltaBuffer = new ArrayBuffer(4 * O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES);
        this.m_idelta = new Float32Array(this.m_ideltaBuffer);
        var m_minMaxBuffer = new ArrayBuffer(32);
        this.m_minNormal = new Float32Array(m_minMaxBuffer, 0, 2);
        this.m_maxNormal = new Float32Array(m_minMaxBuffer, 8, 2);
        this.m_minNormal[0] = this.m_minNormal[1] = -2;
        this.m_maxNormal[0] = this.m_maxNormal[1] = 2;
        for (var i = 0; i < O3DGC_SC3DMC_MAX_DIM_FLOAT_ATTRIBUTES; ++i) {
            this.m_neighbors[i] = new module.SC3DMCPredictor();
        }
    }
    module.SC3DMCDecoder.prototype.GetStats = function () {
        return this.m_stats;
    }
    module.SC3DMCDecoder.prototype.DecodeHeader = function (ifs, bstream) {
        var c0 = this.m_iterator.m_count;
        var start_code = bstream.ReadUInt32(this.m_iterator, O3DGC_STREAM_TYPE_BINARY);
        if (start_code !== O3DGC_SC3DMC_START_CODE) {
            this.m_iterator.m_count = c0;
            start_code = bstream.ReadUInt32(this.m_iterator, O3DGC_STREAM_TYPE_ASCII);
            if (start_code !== O3DGC_SC3DMC_START_CODE) {
                return O3DGC_ERROR_CORRUPTED_STREAM;
            }
            else {
                this.m_streamType = O3DGC_STREAM_TYPE_ASCII;
            }
        }
        else {
            this.m_streamType = O3DGC_STREAM_TYPE_BINARY;
        }
        this.m_streamSize = bstream.ReadUInt32(this.m_iterator, this.m_streamType);
        this.m_params.SetEncodeMode(bstream.ReadUChar(this.m_iterator, this.m_streamType));

        ifs.SetCreaseAngle(bstream.ReadFloat32(this.m_iterator, this.m_streamType));
        var mask = bstream.ReadUChar(this.m_iterator, this.m_streamType);
        ifs.SetCCW((mask & 1) === 1);
        ifs.SetSolid((mask & 2) === 1);
        ifs.SetConvex((mask & 4) === 1);
        ifs.SetIsTriangularMesh((mask & 8) === 1);

        ifs.SetNCoord(bstream.ReadUInt32(this.m_iterator, this.m_streamType));
        ifs.SetNNormal(bstream.ReadUInt32(this.m_iterator, this.m_streamType));
        ifs.SetNumFloatAttributes(bstream.ReadUInt32(this.m_iterator, this.m_streamType));
        ifs.SetNumIntAttributes(bstream.ReadUInt32(this.m_iterator, this.m_streamType));

        if (ifs.GetNCoord() > 0) {
            ifs.SetNCoordIndex(bstream.ReadUInt32(this.m_iterator, this.m_streamType));
            for (var j = 0; j < 3; ++j) {
                ifs.SetCoordMin(j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
                ifs.SetCoordMax(j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
            }
            this.m_params.SetCoordQuantBits(bstream.ReadUChar(this.m_iterator, this.m_streamType));
        }
        if (ifs.GetNNormal() > 0) {
            ifs.SetNNormalIndex(bstream.ReadUInt32(this.m_iterator, this.m_streamType));
            for (var j = 0; j < 3; ++j) {
                ifs.SetNormalMin(j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
                ifs.SetNormalMax(j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
            }
            ifs.SetNormalPerVertex(bstream.ReadUChar(this.m_iterator, this.m_streamType) === 1);
            this.m_params.SetNormalQuantBits(bstream.ReadUChar(this.m_iterator, this.m_streamType));
        }
        for (var a = 0; a < ifs.GetNumFloatAttributes(); ++a) {
            ifs.SetNFloatAttribute(a, bstream.ReadUInt32(this.m_iterator, this.m_streamType));
            if (ifs.GetNFloatAttribute(a) > 0) {
                ifs.SetNFloatAttributeIndex(a, bstream.ReadUInt32(this.m_iterator, this.m_streamType));
                var d = bstream.ReadUChar(this.m_iterator, this.m_streamType);
                ifs.SetFloatAttributeDim(a, d);
                for (var j = 0; j < d; ++j) {
                    ifs.SetFloatAttributeMin(a, j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
                    ifs.SetFloatAttributeMax(a, j, bstream.ReadFloat32(this.m_iterator, this.m_streamType));
                }
                ifs.SetFloatAttributePerVertex(a, bstream.ReadUChar(this.m_iterator, this.m_streamType) === 1);
                ifs.SetFloatAttributeType(a, bstream.ReadUChar(this.m_iterator, this.m_streamType));
                this.m_params.SetFloatAttributeQuantBits(a, bstream.ReadUChar(this.m_iterator, this.m_streamType));
            }
        }
        for (var a = 0; a < ifs.GetNumIntAttributes(); ++a) {
            ifs.SetNIntAttribute(a, bstream.ReadUInt32(this.m_iterator, this.m_streamType));
            if (ifs.GetNIntAttribute(a) > 0) {
                ifs.SetNIntAttributeIndex(a, bstream.ReadUInt32(this.m_iterator, this.m_streamType));
                ifs.SetIntAttributeDim(a, bstream.ReadUChar(this.m_iterator, this.m_streamType));
                ifs.SetIntAttributePerVertex(a, bstream.ReadUChar(this.m_iterator, this.m_streamType) === 1);
                ifs.SetIntAttributeType(a, bstream.ReadUChar(this.m_iterator, this.m_streamType));
            }
        }
        return O3DGC_OK;
    }
    module.SC3DMCDecoder.prototype.DecodeIntArray = function (intArray,
                                                              numIntArray,
                                                              dimIntArray,
                                                              stride,
                                                              bstream) {
        var _iterator = this.m_iterator;
        var _streamType = this.m_streamType;
        var nvert = numIntArray;
        var acd = new module.ArithmeticDecoder();
        var bModel0 = new module.StaticBitModel();
        var bModel1 = new module.AdaptiveBitModel();
        var start = _iterator.m_count;
        var sizeSize = bstream.ReadUInt32(_iterator, _streamType); // bitsream size
        bstream.ReadUChar(_iterator, _streamType); // unsigned char mask = bstream.ReadUChar(_iterator, _streamType);
        var exp_k;
        var M = 0;
        var minValue = bstream.ReadUInt32(_iterator, _streamType) - O3DGC_MAX_LONG;
        sizeSize -= (_iterator.m_count - start);
        if (_streamType !== O3DGC_STREAM_TYPE_ASCII) {
            var buffer = bstream.GetBuffer(_iterator, sizeSize);
            _iterator.m_count += sizeSize;
            acd.SetBuffer(sizeSize, buffer);
            acd.StartDecoder();
            exp_k = acd.ExpGolombDecode(0, bModel0, bModel1);
            M = acd.ExpGolombDecode(0, bModel0, bModel1);
        }
        var mModelValues = new module.AdaptiveDataModel();
        mModelValues.SetAlphabet(M + 2);
        if (_streamType === O3DGC_STREAM_TYPE_ASCII) {
            for (var v = 0; v < nvert; ++v) {
                for (var i = 0; i < dimIntArray; ++i) {
                    intArray[v * stride + i] = bstream.ReadUIntASCII(_iterator) + minValue;
                }
            }
        }
        else {
            for (var v = 0; v < nvert; ++v) {
                for (var i = 0; i < dimIntArray; ++i) {
                    intArray[v * stride + i] = acd.DecodeUIntACEGC(mModelValues, bModel0, bModel1, exp_k, M) + minValue;
                }
            }
        }
        return O3DGC_OK;
    }
    module.SC3DMCDecoder.prototype.ProcessNormals = function (ifs) {
        var nvert = ifs.GetNNormal();
        var normalSize = ifs.GetNNormal() * 2;
        if (this.m_normalsSize < normalSize) {
            this.m_normalsBuffer = new ArrayBuffer(4 * normalSize);
            this.m_normals = new Float32Array(this.m_normalsBuffer);
        }
        var _normals = this.m_normals;
        var _quantFloatArray = this.m_quantFloatArray;
        var _orientation = this.m_orientation;
        var v2T = this.m_triangleListDecoder.GetVertexToTriangle();
        var triangles = ifs.GetCoordIndex();
        var d1 = new module.Vec3();
        var d2 = new module.Vec3();
        var n0 = new module.Vec3();
        var n1 = new module.Vec3();
        for (var v = 0; v < nvert; ++v) {
            n0.Set(0, 0, 0);
            var u0 = v2T.Begin(v);
            var u1 = v2T.End(v);
            for (var u = u0; u < u1; ++u) {
                var ta = v2T.GetNeighbor(u);
                if (ta === -1) {
                    break;
                }
                a = triangles[ta * 3 + 0];
                b = triangles[ta * 3 + 1];
                c = triangles[ta * 3 + 2];
                d1.m_x = _quantFloatArray[3 * b] - _quantFloatArray[3 * a];
                d1.m_y = _quantFloatArray[3 * b + 1] - _quantFloatArray[3 * a + 1];
                d1.m_z = _quantFloatArray[3 * b + 2] - _quantFloatArray[3 * a + 2];
                d2.m_x = _quantFloatArray[3 * c] - _quantFloatArray[3 * a];
                d2.m_y = _quantFloatArray[3 * c + 1] - _quantFloatArray[3 * a + 1];
                d2.m_z = _quantFloatArray[3 * c + 2] - _quantFloatArray[3 * a + 2];
                n0.m_x += d1.m_y * d2.m_z - d1.m_z * d2.m_y;
                n0.m_y += d1.m_z * d2.m_x - d1.m_x * d2.m_z;
                n0.m_z += d1.m_x * d2.m_y - d1.m_y * d2.m_x;
            }
            norm0 = Math.sqrt(n0.m_x * n0.m_x + n0.m_y * n0.m_y + n0.m_z * n0.m_z);
            if (norm0 === 0.0) {
                norm0 = 1.0;
            }
            SphereToCube(n0, n1);
            var rna0 = n1.m_x / norm0;
            var rnb0 = n1.m_y / norm0;
            var ni1 = n1.m_z + _orientation[v];
            _orientation[v] = ni1;
            if ((ni1 >>> 1) !== (n1.m_z >>> 1)) {
                rna0 = 0.0;
                rnb0 = 0.0;
            }
            _normals[2 * v] = rna0;
            _normals[2 * v + 1] = rnb0;
        }
        return O3DGC_OK;
    }
    module.SC3DMCDecoder.prototype.DecodeFloatArrayBinary = function (floatArray,
                                                                      numFloatArray,
                                                                      dimFloatArray,
                                                                      stride,
                                                                      minFloatArray,
                                                                      maxFloatArray,
                                                                      nQBits,
                                                                      ifs,
                                                                      predMode,
                                                                      bstream) {
        var _iterator = this.m_iterator;
        var _orientation = this.m_orientation;
        var _streamType = this.m_streamType;
        var predResidual;
        var acd = new module.ArithmeticDecoder();
        var bModel0 = new module.StaticBitModel();
        var bModel1 = new module.AdaptiveBitModel();
        var mModelPreds = new module.AdaptiveDataModel();
        mModelPreds.SetAlphabet(O3DGC_SC3DMC_MAX_PREDICTION_NEIGHBORS + 1);
        var v2T = this.m_triangleListDecoder.GetVertexToTriangle();
        var triangles = ifs.GetCoordIndex();
        var nvert = numFloatArray;
        var size = numFloatArray * dimFloatArray;
        var start = _iterator.m_count;
        var streamSize = bstream.ReadUInt32(_iterator, _streamType);        // bitsream size
        var mask = bstream.ReadUChar(_iterator, _streamType);
        var binarization = (mask >>> 4) & 7;
        predMode.m_value = mask & 7;
        streamSize -= (_iterator.m_count - start);
        var iteratorPred = new module.Iterator();
        iteratorPred.m_count = _iterator.m_count + streamSize;
        var exp_k = 0;
        var M = 0;
        var nin = new module.Vec3(0, 0, 0);
        var nout = new module.Vec3(0, 0, 0);
        var id = new module.SC3DMCTriplet(-1, -1, -1);

        if (binarization !== O3DGC_SC3DMC_BINARIZATION_AC_EGC) {
            return O3DGC_ERROR_CORRUPTED_STREAM;
        }
        buffer = bstream.GetBuffer(_iterator, streamSize);
        _iterator.m_count += streamSize;
        acd.SetBuffer(streamSize, buffer);
        acd.StartDecoder();
        exp_k = acd.ExpGolombDecode(0, bModel0, bModel1);
        M = acd.ExpGolombDecode(0, bModel0, bModel1);
        var mModelValues = new module.AdaptiveDataModel();
        mModelValues.SetAlphabet(M + 2);

        if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION) {
            if (this.m_orientationSize < size) {
                this.m_orientationBuffer = new ArrayBuffer(numFloatArray);
                this.m_orientation = new Int8Array(this.m_orientationBuffer);
                _orientation = this.m_orientation;
            }
            var dModel = new module.AdaptiveDataModel();
            dModel.SetAlphabet(12);
            for (var i = 0; i < numFloatArray; ++i) {
                _orientation[i] = UIntToInt(acd.DecodeAdaptiveDataModel(dModel));
            }
            this.ProcessNormals(ifs);
            dimFloatArray = 2;
        }

        if (this.m_quantFloatArraySize < size) {
            this.m_quantFloatArrayBuffer = new ArrayBuffer(4 * size);
            this.m_quantFloatArray = new Int32Array(this.m_quantFloatArrayBuffer);
        }
        var _quantFloatArray = this.m_quantFloatArray;
        var _neighbors = this.m_neighbors;
        var _normals = this.m_normals;
        var nPred = new module.NumberRef();
        for (var v = 0; v < nvert; ++v) {
            nPred.m_value = 0;
            if (v2T.GetNumNeighbors(v) > 0 &&
                predMode.m_value !== O3DGC_SC3DMC_NO_PREDICTION) {
                var u_begin = v2T.Begin(v);
                var u_end = v2T.End(v);
                for (var u = u_begin; u < u_end; ++u) {
                    var ta = v2T.GetNeighbor(u);
                    if (ta < 0) {
                        break;
                    }
                    if (predMode.m_value === O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION) {
                        var a, b;
                        if (triangles[ta * 3] === v) {
                            a = triangles[ta * 3 + 1];
                            b = triangles[ta * 3 + 2];
                        }
                        else if (triangles[ta * 3 + 1] === v) {
                            a = triangles[ta * 3 + 0];
                            b = triangles[ta * 3 + 2];
                        }
                        else {
                            a = triangles[ta * 3 + 0];
                            b = triangles[ta * 3 + 1];
                        }
                        if (a < v && b < v) {
                            var u1_begin = v2T.Begin(a);
                            var u1_end = v2T.End(a);
                            for (var u1 = u1_begin; u1 < u1_end; ++u1) {
                                var tb = v2T.GetNeighbor(u1);
                                if (tb < 0) {
                                    break;
                                }
                                var c = -1;
                                var foundB = false;
                                for (var k = 0; k < 3; ++k) {
                                    var x = triangles[tb * 3 + k];
                                    if (x === b) {
                                        foundB = true;
                                    }
                                    if (x < v && x !== a && x !== b) {
                                        c = x;
                                    }
                                }
                                if (c !== -1 && foundB) {
                                    id.m_a = Math.min(a, b);
                                    id.m_b = Math.max(a, b);
                                    id.m_c = (-c - 1);
                                    var p = InsertPredictor(id, nPred, _neighbors, dimFloatArray);
                                    if (p !== 0xFFFFFFFF) {
                                        for (var i = 0; i < dimFloatArray; ++i) {
                                            _neighbors[p].m_pred[i] = _quantFloatArray[a * stride + i] +
                                                                      _quantFloatArray[b * stride + i] -
                                                                      _quantFloatArray[c * stride + i];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION ||
                        predMode.m_value === O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION ||
                        predMode.m_value === O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION) {
                        for (var k = 0; k < 3; ++k) {
                            var w = triangles[ta * 3 + k];
                            if (w < v) {
                                id.m_a = -1;
                                id.m_b = -1;
                                id.m_c = w;
                                var p = InsertPredictor(id, nPred, _neighbors, dimFloatArray);
                                if (p !== 0xFFFFFFFF) {
                                    for (var i = 0; i < dimFloatArray; ++i) {
                                        _neighbors[p].m_pred[i] = _quantFloatArray[w * stride + i];
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (nPred.m_value > 1) {
                var bestPred = acd.DecodeAdaptiveDataModel(mModelPreds);
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = acd.DecodeIntACEGC(mModelValues, bModel0, bModel1, exp_k, M);
                    _quantFloatArray[v * stride + i] = predResidual + _neighbors[bestPred].m_pred[i];
                }
            }
            else if (v > 0 && predMode.m_value !== O3DGC_SC3DMC_NO_PREDICTION) {
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = acd.DecodeIntACEGC(mModelValues, bModel0, bModel1, exp_k, M);
                    _quantFloatArray[v * stride + i] = predResidual + _quantFloatArray[(v - 1) * stride + i];
                }
            }
            else {
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = acd.DecodeUIntACEGC(mModelValues, bModel0, bModel1, exp_k, M);
                    _quantFloatArray[v * stride + i] = predResidual;
                }
            }
        }
        _iterator.m_count = iteratorPred.m_count;
        if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION) {
            this.IQuantizeFloatArray(floatArray, numFloatArray, dimFloatArray, stride, this.m_minNormal, this.m_maxNormal, nQBits + 1);
            for (var v = 0; v < nvert; ++v) {
                nin.m_x = floatArray[stride * v] + _normals[2 * v];
                nin.m_y = floatArray[stride * v + 1] + _normals[2 * v + 1];
                nin.m_z = _orientation[v];
                CubeToSphere[nin.m_z](nin, nout);
                floatArray[stride * v] = nout.m_x;
                floatArray[stride * v + 1] = nout.m_y;
                floatArray[stride * v + 2] = nout.m_z;
            }
        }
        else {
            this.IQuantizeFloatArray(floatArray, numFloatArray, dimFloatArray, stride, minFloatArray, maxFloatArray, nQBits);
        }
        return O3DGC_OK;
    }



    module.SC3DMCDecoder.prototype.DecodeFloatArrayASCII = function (floatArray,
                                                                     numFloatArray,
                                                                     dimFloatArray,
                                                                     stride,
                                                                     minFloatArray,
                                                                     maxFloatArray,
                                                                     nQBits,
                                                                     ifs,
                                                                     predMode,
                                                                     bstream) {
        var _iterator = this.m_iterator;
        var _orientation = this.m_orientation;
        var _streamType = this.m_streamType;
        var predResidual;
        var v2T = this.m_triangleListDecoder.GetVertexToTriangle();
        var triangles = ifs.GetCoordIndex();
        var nvert = numFloatArray;
        var size = numFloatArray * dimFloatArray;
        var start = _iterator.m_count;
        var streamSize = bstream.ReadUInt32(_iterator, _streamType);        // bitsream size
        var mask = bstream.ReadUChar(_iterator, _streamType);
        var binarization = (mask >>> 4) & 7;
        predMode.m_value = mask & 7;
        streamSize -= (_iterator.m_count - start);
        var iteratorPred = new module.Iterator();
        iteratorPred.m_count = _iterator.m_count + streamSize;
        var nin = new module.Vec3(0, 0, 0);
        var nout = new module.Vec3(0, 0, 0);
        var id = new module.SC3DMCTriplet(-1, -1, -1);

        if (binarization !== O3DGC_SC3DMC_BINARIZATION_ASCII) {
            return O3DGC_ERROR_CORRUPTED_STREAM;
        }
        bstream.ReadUInt32(iteratorPred, _streamType);        // predictors bitsream size

        if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION) {
            if (this.m_orientationSize < size) {
                this.m_orientationBuffer = new ArrayBuffer(numFloatArray);
                this.m_orientation = new Int8Array(this.m_orientationBuffer);
                _orientation = this.m_orientation;
            }
            for (var i = 0; i < numFloatArray; ++i) {
                _orientation[i] = bstream.ReadIntASCII(_iterator);
            }
            this.ProcessNormals(ifs);
            dimFloatArray = 2;
        }

        if (this.m_quantFloatArraySize < size) {
            this.m_quantFloatArrayBuffer = new ArrayBuffer(4 * size);
            this.m_quantFloatArray = new Int32Array(this.m_quantFloatArrayBuffer);
        }
        var _quantFloatArray = this.m_quantFloatArray;
        var _neighbors = this.m_neighbors;
        var _normals = this.m_normals;
        var nPred = new module.NumberRef();
        for (var v = 0; v < nvert; ++v) {
            nPred.m_value = 0;
            if (v2T.GetNumNeighbors(v) > 0 &&
                predMode.m_value !== O3DGC_SC3DMC_NO_PREDICTION) {
                var u_begin = v2T.Begin(v);
                var u_end = v2T.End(v);
                for (var u = u_begin; u < u_end; ++u) {
                    var ta = v2T.GetNeighbor(u);
                    if (ta < 0) {
                        break;
                    }
                    if (predMode.m_value === O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION) {
                        var a, b;
                        if (triangles[ta * 3] === v) {
                            a = triangles[ta * 3 + 1];
                            b = triangles[ta * 3 + 2];
                        }
                        else if (triangles[ta * 3 + 1] === v) {
                            a = triangles[ta * 3 + 0];
                            b = triangles[ta * 3 + 2];
                        }
                        else {
                            a = triangles[ta * 3 + 0];
                            b = triangles[ta * 3 + 1];
                        }
                        if (a < v && b < v) {
                            var u1_begin = v2T.Begin(a);
                            var u1_end = v2T.End(a);
                            for (var u1 = u1_begin; u1 < u1_end; ++u1) {
                                var tb = v2T.GetNeighbor(u1);
                                if (tb < 0) {
                                    break;
                                }
                                var c = -1;
                                var foundB = false;
                                for (var k = 0; k < 3; ++k) {
                                    var x = triangles[tb * 3 + k];
                                    if (x === b) {
                                        foundB = true;
                                    }
                                    if (x < v && x !== a && x !== b) {
                                        c = x;
                                    }
                                }
                                if (c !== -1 && foundB) {
                                    id.m_a = Math.min(a, b);
                                    id.m_b = Math.max(a, b);
                                    id.m_c = (-c - 1);
                                    var p = InsertPredictor(id, nPred, _neighbors, dimFloatArray);
                                    if (p !== 0xFFFFFFFF) {
                                        for (var i = 0; i < dimFloatArray; ++i) {
                                            _neighbors[p].m_pred[i] = _quantFloatArray[a * stride + i] +
                                                                      _quantFloatArray[b * stride + i] -
                                                                      _quantFloatArray[c * stride + i];
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION ||
                        predMode.m_value === O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION ||
                        predMode.m_value === O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION) {
                        for (var k = 0; k < 3; ++k) {
                            var w = triangles[ta * 3 + k];
                            if (w < v) {
                                id.m_a = -1;
                                id.m_b = -1;
                                id.m_c = w;
                                var p = InsertPredictor(id, nPred, _neighbors, dimFloatArray);
                                if (p !== 0xFFFFFFFF) {
                                    for (var i = 0; i < dimFloatArray; ++i) {
                                        _neighbors[p].m_pred[i] = _quantFloatArray[w * stride + i];
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (nPred.m_value > 1) {
                var bestPred = bstream.ReadUCharASCII(iteratorPred);
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = bstream.ReadIntASCII(_iterator);
                    _quantFloatArray[v * stride + i] = predResidual + _neighbors[bestPred].m_pred[i];
                }
            }
            else if (v > 0 && predMode.m_value !== O3DGC_SC3DMC_NO_PREDICTION) {
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = bstream.ReadIntASCII(_iterator);
                    _quantFloatArray[v * stride + i] = predResidual + _quantFloatArray[(v - 1) * stride + i];
                }
            }
            else {
                for (var i = 0; i < dimFloatArray; ++i) {
                    predResidual = bstream.ReadUIntASCII(_iterator);
                    _quantFloatArray[v * stride + i] = predResidual;
                }
            }
        }
        _iterator.m_count = iteratorPred.m_count;
        if (predMode.m_value === O3DGC_SC3DMC_SURF_NORMALS_PREDICTION) {
            this.IQuantizeFloatArray(floatArray, numFloatArray, dimFloatArray, stride, this.m_minNormal, this.m_maxNormal, nQBits + 1);
            for (var v = 0; v < nvert; ++v) {
                nin.m_x = floatArray[stride * v] + _normals[2 * v];
                nin.m_y = floatArray[stride * v + 1] + _normals[2 * v + 1];
                nin.m_z = _orientation[v];
                CubeToSphere[nin.m_z](nin, nout);
                floatArray[stride * v] = nout.m_x;
                floatArray[stride * v + 1] = nout.m_y;
                floatArray[stride * v + 2] = nout.m_z;
            }
        }
        else {
            this.IQuantizeFloatArray(floatArray, numFloatArray, dimFloatArray, stride, minFloatArray, maxFloatArray, nQBits);
        }
        return O3DGC_OK;
    }
    module.SC3DMCDecoder.prototype.DecodeFloatArray = function (floatArray,
                                                                numFloatArray,
                                                                dimFloatArray,
                                                                stride,
                                                                minFloatArray,
                                                                maxFloatArray,
                                                                nQBits,
                                                                ifs,
                                                                predMode,
                                                                bstream) {
        if (this.m_streamType === O3DGC_STREAM_TYPE_ASCII) {
            return this.DecodeFloatArrayASCII(floatArray, numFloatArray, dimFloatArray, stride, minFloatArray, maxFloatArray, nQBits, ifs, predMode, bstream);
        }
        else {
            return this.DecodeFloatArrayBinary(floatArray, numFloatArray, dimFloatArray, stride, minFloatArray, maxFloatArray, nQBits, ifs, predMode, bstream);
        }
    }
    module.SC3DMCDecoder.prototype.IQuantizeFloatArray = function (floatArray, numFloatArray, dimFloatArray, stride, minFloatArray, maxFloatArray, nQBits) {
        var _idelta = this.m_idelta;
        var _quantFloatArray = this.m_quantFloatArray;
        for (var d = 0; d < dimFloatArray; ++d) {
            var r = maxFloatArray[d] - minFloatArray[d];
            if (r > 0.0) {
                _idelta[d] = r / (((1 << nQBits) >>> 0) - 1);
            }
            else {
                _idelta[d] = 1.0;
            }
        }
        for (var v = 0; v < numFloatArray; ++v) {
            for (var d = 0; d < dimFloatArray; ++d) {
                floatArray[v * stride + d] = _quantFloatArray[v * stride + d] * _idelta[d] + minFloatArray[d];
            }
        }
        return O3DGC_OK;
    }
    module.SC3DMCDecoder.prototype.DecodePlayload = function (ifs, bstream) {
        var _params = this.m_params;
        var _iterator = this.m_iterator;
        var _stats = this.m_stats;
        var predMode = new module.NumberRef();
        var timer = new module.Timer();
        var ret = O3DGC_OK;

        this.m_triangleListDecoder.SetStreamType(this.m_streamType);
        _stats.m_streamSizeCoordIndex = _iterator.m_count;
        timer.Tic();
        this.m_triangleListDecoder.Decode(ifs.GetCoordIndex(), ifs.GetNCoordIndex(), ifs.GetNCoord(), bstream, _iterator);
        timer.Toc();

        _stats.m_timeCoordIndex = timer.GetElapsedTime();
        _stats.m_streamSizeCoordIndex = _iterator.m_count - _stats.m_streamSizeCoordIndex;

        // decode coord
        _stats.m_streamSizeCoord = _iterator.m_count;
        timer.Tic();
        if (ifs.GetNCoord() > 0) {
            ret = this.DecodeFloatArray(ifs.GetCoord(), ifs.GetNCoord(), 3, 3, ifs.GetCoordMinArray(), ifs.GetCoordMaxArray(),
                                   _params.GetCoordQuantBits(), ifs, predMode, bstream);
            _params.SetCoordPredMode(predMode.m_value);
        }
        if (ret !== O3DGC_OK) {
            return ret;
        }
        timer.Toc();
        _stats.m_timeCoord = timer.GetElapsedTime();
        _stats.m_streamSizeCoord = _iterator.m_count - _stats.m_streamSizeCoord;

        // decode Normal
        _stats.m_streamSizeNormal = _iterator.m_count;
        timer.Tic();
        if (ifs.GetNNormal() > 0) {
            ret = this.DecodeFloatArray(ifs.GetNormal(), ifs.GetNNormal(), 3, 3, ifs.GetNormalMinArray(), ifs.GetNormalMaxArray(),
                                _params.GetNormalQuantBits(), ifs, predMode, bstream);
            _params.SetNormalPredMode(predMode.m_value);
        }
        if (ret !== O3DGC_OK) {
            return ret;
        }
        timer.Toc();
        _stats.m_timeNormal = timer.GetElapsedTime();
        _stats.m_streamSizeNormal = _iterator.m_count - _stats.m_streamSizeNormal;

        // decode FloatAttributes
        for (var a = 0; a < ifs.GetNumFloatAttributes(); ++a) {
            _stats.m_streamSizeFloatAttribute[a] = _iterator.m_count;
            timer.Tic();
            ret = this.DecodeFloatArray(ifs.GetFloatAttribute(a), ifs.GetNFloatAttribute(a), ifs.GetFloatAttributeDim(a), ifs.GetFloatAttributeDim(a),
                                ifs.GetFloatAttributeMinArray(a), ifs.GetFloatAttributeMaxArray(a),
                                _params.GetFloatAttributeQuantBits(a), ifs, predMode, bstream);
            _params.SetFloatAttributePredMode(a, predMode.m_value);
            timer.Toc();
            _stats.m_timeFloatAttribute[a] = timer.GetElapsedTime();
            _stats.m_streamSizeFloatAttribute[a] = _iterator.m_count - _stats.m_streamSizeFloatAttribute[a];
        }
        if (ret !== O3DGC_OK) {
            return ret;
        }

        // decode FloatAttributes
        for (var a = 0; a < ifs.GetNumIntAttributes(); ++a) {
            _stats.m_streamSizeIntAttribute[a] = _iterator.m_count;
            timer.Tic();
            ret = this.DecodeIntArray(ifs.GetIntAttribute(a), ifs.GetNIntAttribute(a), ifs.GetIntAttributeDim(a), ifs.GetIntAttributeDim(a), bstream);
            timer.Toc();
            _stats.m_timeIntAttribute[a] = timer.GetElapsedTime();
            _stats.m_streamSizeIntAttribute[a] = _iterator.m_count - _stats.m_streamSizeIntAttribute[a];
        }
        if (ret !== O3DGC_OK) {
            return ret;
        }

        timer.Tic();
        this.m_triangleListDecoder.Reorder();
        timer.Toc();
        _stats.m_timeReorder = timer.GetElapsedTime();

        return ret;
    }

    return module;
})();

