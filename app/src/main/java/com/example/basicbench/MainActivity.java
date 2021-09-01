package com.example.basicbench;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import com.example.basicbench.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class MainActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private ActivityMainBinding binding;
    private TextView tv;

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        List<String> typeList = new ArrayList<>();
        DataTypesMap.initMap();
        //DataTypesMap.dataTypesStringmap.forEach((k,v) -> {
        //    typeList.add(v);}
        //    );
        for (Map.Entry<DataTypes, String> entry : DataTypesMap.dataTypesStringmap.entrySet()) {
            typeList.add(entry.getValue());
        }
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        ArrayAdapter arrayAdapter = new ArrayAdapter(this, android.R.layout.simple_spinner_item, typeList.toArray());
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        tv = findViewById(R.id.sample_text);
        tv.setText("AAA");
        Spinner spin = (Spinner) findViewById(R.id.spinner3);
        spin.setOnItemSelectedListener(this);
        spin.setAdapter(arrayAdapter);
        setContentView(binding.getRoot());
        initOCL();

        Button bt = (Button) findViewById(R.id.button);
        bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runTestFlops();
            }
        });
        Button bt2 = (Button) findViewById(R.id.button2);
        bt2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runBandwith();
            }
        });
        Button bt5 = (Button) findViewById(R.id.button5);
        bt5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runLatency();
            }
        });
        Button bt6 = (Button) findViewById(R.id.button6);
        bt6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runReadWrite();
            }
        });
        Button bt3 = (Button) findViewById(R.id.button3);
        bt3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                tv.setText(runMatrixMul());
            }
        });
        Button bt4 = (Button) findViewById(R.id.button4);
        bt4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runAllInLog();
            }
        });
    }

    private void runAllInLog() {
        Log.w("Test:","Flops");
        for (Map.Entry<DataTypes, String> entry : DataTypesMap.dataTypesStringmap.entrySet()) {
            int typeOrdinal = (entry.getKey().ordinal());
            double flops = runFlopsTest(typeOrdinal);
            String typeString = "";
            if (typeOrdinal / 5 == 0) {
                typeString = "HLOPS";
            } else if (typeOrdinal / 5 == 1) {
                typeString = "ILOPS";
            } else if (typeOrdinal / 5 == 2) {
                typeString = "FLOPS";
            } else if (typeOrdinal / 5 == 3) {
                typeString = "DLOPS";
            }
            Log.w(entry.getValue(), ": " + Double.toString(flops / (1024 * 1024 * 1024)) + " G" + typeString);
        }
        Log.w("Test:","Bandwith");
        for (Map.Entry<DataTypes, String> entry : DataTypesMap.dataTypesStringmap.entrySet()) {
            double result = runBandwith((entry.getKey().ordinal()));
            Log.w(entry.getValue(),": " + Double.toString(result / (1024.0 * 1024.0 * 1024.0)) + " GB/s");
        }
        Log.w("Test:","Latency");
        double result = runOCLLatency();
        Log.w("Latency",": " + Double.toString(result)+ " ns");
        Log.w("Test:","Read/Write/Map/UnMap");
        result = 0.0;
        result = runReadBuffer();
        Log.w("clEnqueueReadBuffer: " , Double.toString(result / 1024.0) + " GB/s\n");
        result = runWriteBuffer();
        Log.w("clEnqueueWriteBuffer: ", Double.toString(result / 1024.0) + " GB/s\n");
        result = runMapBuffer();
        Log.w("clEnqueueMapBuffer: ", Double.toString(result / 1024.0) + " GB/s\n");
        result = runUnMapBuffer();
        Log.w("clEnqueueUnMapBuffer: ", Double.toString(result / 1024.0) + " GB/s\n");
        Log.w("Test:","Matrix Mul");
        Log.w("Result:",runMatrixMul());
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onDestroy() {
        closeOCL();
        super.onDestroy();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void initOCL();

    public native void closeOCL();

    public native double runFlopsTest(int dataType);

    public native double runBandwith(int dataType);

    public native double runReadBuffer();

    public native double runWriteBuffer();

    public native double runMapBuffer();

    public native double runUnMapBuffer();

    public native double runOCLLatency();

    public native String runMatrixMul();

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        Spinner spinner = (Spinner) findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        tv = findViewById(R.id.sample_text);
        tv.setText(text);

    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }

    public void runTestFlops() {
        Spinner spinner = (Spinner) findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        DataTypes type;
        double flops = 0.0;
        int typeOrdinal = 0;
        for (Map.Entry<DataTypes, String> entry : DataTypesMap.dataTypesStringmap.entrySet()) {
            if (text.compareTo(entry.getValue()) == 0) {
                typeOrdinal = (entry.getKey().ordinal());
                flops = runFlopsTest(typeOrdinal);
                break;
            }
        }
        String typeString = "";
        if (typeOrdinal / 5 == 0) {
            typeString = "HLOPS";
        } else if (typeOrdinal / 5 == 1) {
            typeString = "ILOPS";
        } else if (typeOrdinal / 5 == 2) {
            typeString = "FLOPS";
        } else if (typeOrdinal / 5 == 3) {
            typeString = "DLOPS";
        }
        tv.setText(text + ": " + Double.toString(flops / (1024 * 1024 * 1024)) + " G" + typeString);
    }

    public void runBandwith() {
        Spinner spinner = (Spinner) findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        DataTypes type;
        double result = 0.0;
        for (Map.Entry<DataTypes, String> entry : DataTypesMap.dataTypesStringmap.entrySet()) {
            if (text.compareTo(entry.getValue()) == 0) {
                result = runBandwith((entry.getKey().ordinal()));
                break;
            }
        }
        tv.setText(text + ": " + Double.toString(result / (1024.0 * 1024.0 * 1024.0)) + " GB/s");
    }

    public void runLatency() {
        double result = 0.0;
        result = runOCLLatency();
        String text = "Latency: ";
        tv.setText(text + Double.toString(result) + " ns");
    }

    public void runReadWrite() {
        double result = 0.0;
        result = runReadBuffer();
        String text = "clEnqueueReadBuffer: " + Double.toString(result / 1024.0) + " GB/s\n";
        result = runWriteBuffer();
        text += "clEnqueueWriteBuffer: " + Double.toString(result / 1024.0) + " GB/s\n";
        result = runMapBuffer();
        text += "clEnqueueMapBuffer: " + Double.toString(result / 1024.0) + " GB/s\n";
        result = runUnMapBuffer();
        text += "clEnqueueUnMapBuffer: " + Double.toString(result / 1024.0) + " GB/s\n";
        tv.setText(text);
    }
}