import java.awt.*;
import java.applet.*;
import java.util.*;
import java.io.InputStream;
import java.io.DataInputStream;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.net.URL;
import java.net.URLConnection;
import java.net.MalformedURLException;

class readStream extends Thread {
    String TimeString;
    URL    base;
    readStream(URL b)
    {
        base = b;
    }

    public synchronized void SetTimeString(String s)
    {
    TimeString = s;
    }

    public synchronized String GetTimeString()
    {
        return TimeString;
    }

    public void run() {
        URLConnection   conn = null;
        DataInputStream data = null;
        String          time;
        URL             timeURL;

            // when constructing the new url, use the
        // applet base URL to build it
        try {
            timeURL = new URL(
                                    base.getProtocol(),
                                    base.getHost(),
                                    base.getPort(),
                                    "/stream"
                                    );
        }
        catch (MalformedURLException e) {
            SetTimeString("Bad URL");
            timeURL = null;
        }


            // get the time from the server via an input stream
            // connected to the URL
        if (timeURL != null) {
           try {
                conn = timeURL.openConnection();
                data = new DataInputStream(new BufferedInputStream(conn.getInputStream()));
                while(true) {
                    time = data.readLine();

                    // write the new time to the label
                    if (time != null) {
                        SetTimeString(time);
                    }
                    else {
                        SetTimeString("no input");
                    }
                }
            }
            catch(IOException e) {
                SetTimeString("IO Error:" + e.getMessage());
            }
            finally {
                try {
                data.close();
                }
                catch(IOException e) {
                    SetTimeString("IO Error:" + e.getMessage());
                }
                conn = null;
                data = null;
            }

        }
    }
}

public class streaming extends Applet implements Runnable {
    Thread     t;
    boolean    done;
    readStream rs;

    public void run()
    {
        Date            d;
        int             i;

        i = 0;
        while(!done) {
            // get current time value asynchronously
            label1.setText(rs.GetTimeString());

            // count attempts
            i++;
            label5.setText(String.valueOf(i));

            // date and time output
            d = new Date();
            label2.setText(d.toGMTString());

            // give memory a chance to be freed
            d    = null;

            // sleep a little
            try { Thread.sleep(100);}
            catch(InterruptedException e) {}
        }
    }

    public void start()
    {
        if (rs == null) {
            rs = new readStream(getCodeBase());
            rs.start();
        }

        if (t == null) {
            t = new Thread(this);
            done = false;
            t.start();
        }
    }

    public void stop()
    {
        if (t != null) {
            done = true;
            t = null;
        }
        if (rs != null) {
            rs.stop();
            rs = null;
        }
    }


    public void init() {

        super.init();

        //{{INIT_CONTROLS
        setLayout(null);
        addNotify();
        resize(insets().left + insets().right + 430, insets().top + insets().bottom + 207);
        label1=new Label("");
        add(label1);
        label1.reshape(insets().left + 14,insets().top + 8,172,24);
        label3=new Label("Remote Time (tick)");
        add(label3);
        label3.reshape(insets().left + 203,insets().top + 8,210,24);
        label4=new Label("Local Time");
        add(label4);
        label4.reshape(insets().left + 203,insets().top + 48,210,24);
        label2=new Label("");
        add(label2);
        label2.reshape(insets().left + 14,insets().top + 48,175,24);
        label5=new Label("");
        add(label5);
        label5.reshape(insets().left + 14,insets().top + 88,175,24);
        label6=new Label("Loop Count");
        add(label6);
        label6.reshape(insets().left + 203,insets().top + 88,210,24);
        StopButton=new Button("Stop");
        add(StopButton);
        StopButton.resize(StopButton.preferredSize());
        StopButton.move(14,128);
        StartButton=new Button("Run");
        add(StartButton);
        StartButton.resize(StartButton.preferredSize());
        StartButton.move(308,128);
        //}}
    }

    public boolean handleEvent(Event event) {
        if (event.id == Event.ACTION_EVENT && event.target == StartButton) {
                clickedStartButton();
                return true;
        }
        else
        if (event.id == Event.ACTION_EVENT && event.target == StopButton) {
                clickedStopButton();
                return true;
        }

        return super.handleEvent(event);
    }

    //{{DECLARE_CONTROLS
    Label label1;
    Label label3;
    Label label4;
    Label label2;
    Label label5;
    Label label6;
    Button StopButton;
    Button StartButton;
    //}}
    public void clickedStopButton() {
        // suspend update thread but not
        // stream thread so data doesn't back up
        t.suspend();
    }
    public void clickedStartButton() {
        // restart update thread
        t.resume();
    }
}
