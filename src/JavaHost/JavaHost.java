/******************************************************************************
 * Entry point for Java hosted demo
 *
 * This class will load the dotnetbridge native library, which will discover the
 * hostfxr library and load it. When the main() function below runs .NET Core will
 * be activated, an assembly loaded, and a static function on a pre-defined type called.
 ******************************************************************************/
public class JavaHost {
    static {
        // Load the native library
        System.loadLibrary("dotnetbridge");
    }

    public static void main(String[] args) {
        // Call into the .NET Core assembly multiple times
        for (int i = 0; i < 5; ++i) {
            dotnetLibHello();
        }
    }

    // Declare the signature of the native function
    private static native void dotnetLibHello();
}