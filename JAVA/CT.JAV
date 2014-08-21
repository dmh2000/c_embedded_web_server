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

public class ct extends Applet implements Runnable {
    Thread t;
    boolean done;

    public void run()
    {
        URLConnection   conn = null;
        DataInputStream data = null;
        String          time;
        Date            d;
        URL             timeURL;
        int             i;

        // get the base url for this applet
        URL base = getCodeBase();

        // when constructing the new url, use the
        // applet base URL to build it
        try {
            timeURL = new URL(
                                    base.getProtocol(),
                                    base.getHost(),
                                    base.getPort(),
                                    "/systime"
                                    );
            label6.setText(timeURL.toString());
        }
        catch (MalformedURLException e) {
           label1.setText("Bad URL");
           label6.setText("Bad URL");
           timeURL = null;
        }


        i = 0;
        while(!done) {
            // get the time from the server via an input stream
            // connected to the URL
            if (timeURL != null) {
                try {
                    conn = timeURL.openConnection();
                    data = new DataInputStream(new BufferedInputStream(conn.getInputStream()));
                    time = data.readLine();

                    // write the new time to the label
                    if (time != null) {
                        label1.setText(time);
                    }
                    else {
                        label1.setText("no input");
                    }

                    // count attempts
                    i++;
                    label5.setText(String.valueOf(i));
                }
                catch(IOException e) {
                    label1.setText("IO Error:" + e.getMessage());
                    i = 0;
                    label5.setText(String.valueOf(i));
                }
            }

            // date and time output
            d = new Date();
            label2.setText(d.toGMTString());

            // give memory a chance to be freed
            d    = null;
            conn = null;
            data = null;
            try { Thread.sleep(300);}
            catch(InterruptedException e) {}
        }
    }

    public void start()
    {
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
    }


    public void init() {

        super.init();

        //{{INIT_CONTROLS
        setLayout(null);
        addNotify();
        resize(insets().left + insets().right + 428, insets().top + insets().bottom + 170);
        label1=new Label("l1");
        add(label1);
        label1.reshape(insets().left + 14,insets().top + 8,172,24);
        label3=new Label("remote time (ticks)");
        add(label3);
        label3.reshape(insets().left + 203,insets().top + 8,175,24);
        label4=new Label("local time");
        add(label4);
        label4.reshape(insets().left + 203,insets().top + 48,175,24);
        label2=new Label("l4");
        add(label2);
        label2.reshape(insets().left + 14,insets().top + 48,175,24);
        label5=new Label("l5");
        add(label5);
        label5.reshape(insets().left + 14,insets().top + 88,175,24);
        label6=new Label("l6");
        add(label6);
        label6.reshape(insets().left + 203,insets().top + 88,175,24);
        //}}
    }

    public boolean handleEvent(Event event) {
        return super.handleEvent(event);
    }

    //{{DECLARE_CONTROLS
    Label label1;
    Label label3;
    Label label4;
    Label label2;
    Label label5;
    Label label6;
    //}}
}
