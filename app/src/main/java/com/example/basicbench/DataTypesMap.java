package com.example.basicbench;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Collection;
import java.util.Collections;
import java.util.EnumMap;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class DataTypesMap {
    public static Map<DataTypes, String> dataTypesStringmap = null;
    public static void initMap() {
        Map<DataTypes, String> myMap = new EnumMap<DataTypes, String>(DataTypes.class);
        myMap.put(DataTypes.half1,  "Half1");
        myMap.put(DataTypes.half2,  "Half2");
        myMap.put(DataTypes.half4,  "Half4");
        myMap.put(DataTypes.half8,  "Half8");
        myMap.put(DataTypes.half16, "Half16");

        myMap.put(DataTypes.int1,  "Int1");
        myMap.put(DataTypes.int2,  "Int2");
        myMap.put(DataTypes.int4,  "Int4");
        myMap.put(DataTypes.int8,  "Int8");
        myMap.put(DataTypes.int16, "Int16");

        myMap.put(DataTypes.float1,  "Float1");
        myMap.put(DataTypes.float2,  "Float2");
        myMap.put(DataTypes.float4,  "Float4");
        myMap.put(DataTypes.float8,  "Float8");
        myMap.put(DataTypes.float16, "Float16");

        myMap.put(DataTypes.double1,  "Double1");
        myMap.put(DataTypes.double2,  "Double2");
        myMap.put(DataTypes.double4,  "Double4");
        myMap.put(DataTypes.double8,  "Double8");
        myMap.put(DataTypes.double16, "Double16");
        dataTypesStringmap = Collections.unmodifiableMap(myMap);
    }
}
