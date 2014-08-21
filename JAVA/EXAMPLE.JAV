import java.awt.*;
import java.applet.*;

public class example extends Applet {

    public void init() {

        super.init();

        resize(100,30);
        label1=new Label("Hello World");
        add(label1);
        label1.reshape(1,1,99,29);
    }

    Label label1;
}
