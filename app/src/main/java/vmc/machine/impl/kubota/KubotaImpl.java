package vmc.machine.impl.kubota;

import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Button;
import android.view.View.OnClickListener;
import android.view.View;
import android.os.Handler;
import android.os.Message;
import android.widget.LinearLayout;
import android.widget.ScrollView;

public class KubotaImpl extends AppCompatActivity {

    Button btn1, btn2, btn3, btn4, btn5, btn6, btn7, btn8, btn9;
    //TextView tv = (TextView) findViewById(R.id.textView);
    LinearLayout lL;
    ScrollView sV;
    byte columnNo;
    short amount;
    int count;
    String s;
    byte[] salesCount = new byte [4];
    byte[] salesAmount = new byte [4];
    byte[] goodsSoldOutState = new byte [(200 + 7) / 8];
    byte state;
    byte[] faultInformation = new byte [10 * 2];
    byte[] vMN = new byte [4];

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("Serial_kubota");
    }
    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 0x01 :
                    amount = getSelectedGoodAmount();
                    columnNo = getSelectedGoodColumnNo();
                    s = "button is selected, 货道号：";
                    s += String.valueOf(columnNo);
                    s += "，金额：";
                    s += String.valueOf(amount);
                    lL.addView(getTextView(s));
                    break;
                case 0x02 :
                    columnNo = getCurrentSalesColumnNo();
                    salesCount = getCurrentSalesCount();
                    salesAmount = getCurrentSalesAmount();
                    s = "out goods success, 货道号：";
                    s += String.valueOf(columnNo);
                    count = (salesCount[3] & 0xff) | (salesCount[2] & 0xff) << 8 |
                            (salesCount[1] & 0xff) << 16 | (salesCount[0] & 0xff) << 24;
                    s += ",销售数量：";
                    s += String.valueOf(count);
                    s += ",销售金额：";
                    amount = (short)((salesAmount[3] & 0xff) | (salesAmount[2] & 0xff) << 8 |
                            (salesAmount[1] & 0xff) << 16 | (salesAmount[0] & 0xff) << 24);
                    s += String.valueOf(amount);
                    lL.addView(getTextView(s));
                    break;
                case 0x04 :
                    lL.addView(getTextView("ban online trading"));
                    break;
                case 0x08 :
                    lL.addView(getTextView("allows online transaction"));
                    break;
                case 0x10 :
                    lL.addView(getTextView("outGoods fail"));
                    break;
                case 0x20 :
                    if (getSetPriceResults()) {
                        lL.addView(getTextView("set price success"));
                    }
                    else {
                        lL.addView(getTextView("set price fail"));
                    }
                    break;
                case 0x40 :
                    lL.addView(getTextView("transaction end"));
                    break;
                case 0x80 :
                    s = "状态数据：";
                    state = getStateData();
                    s += String.valueOf(state);
                    lL.addView(getTextView(s));
                    break;
                case 0x0100 :
                    s = "故障信息：";
                    count = getFaultInformationCount();
                    faultInformation = getFaultInformation();
                    for (int i = 0; i < count; i++) {
                        s += String.valueOf(faultInformation[i * 2]);
                        s += String.valueOf(faultInformation[i * 2 + 1]);
                        s += ",";
                    }
                    lL.addView(getTextView(s));
                    break;
                case 0x0200 :
                    lL.addView(getTextView("串口没有数据"));
                    break;
                case 0x0400 :
                    goodsSoldOutState = getGoodsSoldOutState();
                    int j = getColumnCount();
                    s = "货道售空状态：";
                    for (int i = 0; i < (j + 7) / 8; i++) {
                        s += String.valueOf(goodsSoldOutState[i]);
                        s += ",";
                    }
                    lL.addView(getTextView(s));
                    break;
                default:
                    break;
            }
            s = "事件号：";
            s += String.valueOf(msg.what);
            lL.addView(getTextView(s));
            final ScrollView svResult = (ScrollView) findViewById(R.id.scrollView1);
            svResult.post(new Runnable() {
                public void run() {
                    svResult.fullScroll(ScrollView.FOCUS_DOWN);
                }
            });
        }
    };

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_kubota_impl);

        lL = (LinearLayout)findViewById(R.id.linearLayout1);
        sV = (ScrollView)findViewById(R.id.scrollView1);
        // Example of a call to a native method

        s = "售货机编号信息：";
        s += String.valueOf(vMN[0]);
        s += String.valueOf(vMN[1]);
        s += String.valueOf(vMN[2]);
        s += String.valueOf(vMN[3]);
        lL.addView(getTextView(s));
        Thread threadEvent = new Thread(new Runnable()
        {
            @Override
            public void run() {
                int i;
                SystemClock.sleep(2000);
                while (true) {
                    i = getEvent();
                    switch (i) {
                        case 0x01 :
                            break;
                        case 0x02 :
                            break;
                        case 0x04 :
                            break;
                        case 0x08 :
                            break;
                        case 0x10 :
                            break;
                        case 0x20 :
                            break;
                        case 0x40 :
                            break;
                        case 0x80 :
                            break;
                        case 0x0100 :
                            break;
                        case 0x0200 :
                            break;
                        case 0x0400 :
                            break;
                    }
                    mHandler.sendEmptyMessage(i);
                }
            }
        });
        threadEvent.start();
        //startProtocol();


        new Thread(){
            @Override
            public void run() {
                SystemClock.sleep(2000);
                byte b[] = "/dev/ttymxc1".getBytes();
                byte[] b1 = new byte [100];
                for (int i = 0; i < 12; i++) {
                    b1[i] = b[1];
                }
                b1[12] = 0;
                startProtocol1(/*"/dev/ttymxc1".getBytes()*/b1, 19200, true);
            }
        }.start();


        vMN = getVendingMachineNumber();
        btn1 = (Button) findViewById(R.id.button);
        btn1.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                outGoods();
            }
        });

        btn2 = (Button) findViewById(R.id.button2);
        btn2.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                outGoodsUseB0ByButton((byte)1);
            }
        });

        btn3 = (Button) findViewById(R.id.button3);
        btn3.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                outGoodsUseB0((byte)1, (byte)1);
            }
        });

        btn4 = (Button) findViewById(R.id.button4);
        btn4.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                int i = getColumnCount();
                byte[] price = new byte [i * 2];

                for (int j = 0; j < i; j++) {
                    price[j * 2] = 0x00;
                    price[j * 2 + 1] = 0x14;
                }
                setColumnPrice(price);
            }
        });

        btn5 = (Button) findViewById(R.id.button5);
        btn5.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                cancelTransaction();
            }
        });

        btn6 = (Button) findViewById(R.id.button6);
        btn6.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                selectGoodsByScreen((byte)1);
            }
        });

        btn7 = (Button) findViewById(R.id.button7);
        btn7.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                selectGoodsByScreen((byte)11);
            }
        });

        btn8 = (Button) findViewById(R.id.button8);
        btn8.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                selectGoodsByScreen((byte)12);
            }
        });

        btn9 = (Button) findViewById(R.id.button9);
        btn9.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                selectGoodsByScreen((byte)13);
            }
        });
    }

    public TextView getTextView(String s) {
        TextView tV = new TextView(KubotaImpl.this);

        tV.setText(s);
        return tV;
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void startProtocol();
    public native void startProtocol1(byte[] portNameIn, int baudRateIn, boolean isDebug);
    public native int getEvent();
    public native short getSelectedGoodAmount();
    public native byte getSelectedGoodColumnNo();
    public native byte getCurrentSalesColumnNo();
    public native byte[] getCurrentSalesCount();
    public native byte[] getCurrentSalesAmount();
    public native boolean getSetPriceResults();
    public native void outGoods();
    public native void outGoodsUseB0ByButton(byte salesCategory);
    public native void outGoodsUseB0(byte columnNo, byte salesCategory);
    public native int getColumnCount();
    public native void setColumnPrice(byte[] price);
    public native void cancelTransaction();
    public native void selectGoodsByScreen(byte columnNo);
    public native byte[] getGoodsSoldOutState();
    public native byte getStateData();
    public native byte[] getFaultInformation();
    public native int getFaultInformationCount();
    public native byte[] getVendingMachineNumber();

}
