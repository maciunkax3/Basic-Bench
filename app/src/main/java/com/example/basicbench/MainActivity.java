package com.example.basicbench;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import com.example.basicbench.databinding.ActivityMainBinding;

import java.util.ArrayList;
import java.util.Collections;
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
        for(Map.Entry<DataTypes, String> entry: DataTypesMap.dataTypesStringmap.entrySet()){
            typeList.add(entry.getValue());
        }
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        ArrayAdapter arrayAdapter = new ArrayAdapter(this,android.R.layout.simple_spinner_item, typeList.toArray());
        arrayAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        tv = findViewById(R.id.sample_text);
        tv.setText("AAA");
        Spinner spin = (Spinner) findViewById(R.id.spinner3);
        spin.setOnItemSelectedListener(this);
        spin.setAdapter(arrayAdapter);
        setContentView(binding.getRoot());
        initOCL();

        Button bt = (Button)findViewById(R.id.button);
        bt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runTestFlops();
            }
        });
        Button bt2 = (Button)findViewById(R.id.button2);
        bt2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                runBandwith();
            }
        });
        // Example of a call to a native method
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

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        Spinner spinner = (Spinner)findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        tv = findViewById(R.id.sample_text);
        tv.setText(text);

    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
    public void runTestFlops(){
        Spinner spinner = (Spinner)findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        DataTypes type;
        double flops = 0.0;
        for(Map.Entry<DataTypes, String> entry: DataTypesMap.dataTypesStringmap.entrySet()){
            if(text.compareTo(entry.getValue()) == 0){
                flops = runFlopsTest((entry.getKey().ordinal()));
                break;
            }
        }
        tv.setText(text + ": "+ Double.toString(flops));
    }
    public void runBandwith(){
        Spinner spinner = (Spinner)findViewById(R.id.spinner3);
        String text = spinner.getSelectedItem().toString();
        DataTypes type;
        double result = 0.0;
        for(Map.Entry<DataTypes, String> entry: DataTypesMap.dataTypesStringmap.entrySet()){
            if(text.compareTo(entry.getValue()) == 0){
                result = runBandwith((entry.getKey().ordinal()));
                break;
            }
        }
        tv.setText(text + ": "+ Double.toString(result));
    }

}