
using System;
using Gtk;
using Gdk;

// Compile with:
// mcs -pkg:gtk-sharp SamplePlug.cs
// mkbundle -o SamplePlug SamplePlug.exe 

public class SamplePlug
{

	public static void Main(string[] args) {
	    if (args.Length != 2) {
		Console.WriteLine("Need socket id and file-name as an argument.");
		return;
	    }
	    uint socket_id = UInt32.Parse(args[0]);
	    string filename=args[1];
	    Console.WriteLine("filename="+filename);
	    
	    Console.WriteLine("using socket "+socket_id);

//	    Glib.Thread.Init();
	    Gdk.Threads.Init();
	    
	    Application.Init();
	    Gdk.Threads.Enter();
	    try {	    

	    Plug plug= new Plug(socket_id);

	    Fixed fixed1 = new Fixed();
	    fixed1.Put(new Label("File: \""+filename+"\""), 10, 10);
	    fixed1.Put(new Entry("HELLO"), 10, 50);
	    fixed1.Put(new Entry("World"), 10, 100);
	    fixed1.ShowAll();
	    plug.Add(fixed1);
	    plug.ShowAll();
	    
	    Console.WriteLine("app is running..");
	    Application.Run();
            } finally {
	      Gdk.Threads.Leave();
            }
	    Console.WriteLine("Done!");
	}	


}
