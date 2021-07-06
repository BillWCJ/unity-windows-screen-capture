using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Threading;

public class Video : MonoBehaviour
{
	private Grabber grabber;
	private Renderer rend;
	private Texture2D texture;

    private GameObject sphere;
    private Vector3 scaleChange, positionChange;

    public int Width, Height;

	// Use this for initialization
	void Start()
	{

	}

    void OnDisable()
	{
        Thread thread = Thread.CurrentThread;
		Debug.Log("PrintOnDisable: script was disabled");

        if (grabber != null)
		{
            Debug.Log("freeing grabber");
			grabber.Dispose();
			grabber = null;
        }
    }

    void OnEnable()
    {
        Thread thread = Thread.CurrentThread;
        Debug.Log("PrintOnEnable: script was enabled");
        rend = GetComponent<Renderer>();
        rend.enabled = true;

        var materials = rend.materials;
        texture = new Texture2D(1, 1, TextureFormat.RGBA32, false, false);
        /*
        foreach (var mat in materials) {
            Debug.Log ("Material: " + mat.name);
        }
        var w = 100;
        var h = 100;
        texture = new Texture2D (w, h, TextureFormat.RGBA32, false, false);
        materials [0].mainTexture = texture;
        for (int i = 0; i < w; i++) {
            for (int j = 0; j < h; j++) {
                texture.SetPixel (i, j, Color.green);
            }
        }
        texture.Apply (true);
        */
        grabber = new Grabber(texture.GetNativeTexturePtr());
        Debug.Log("Width: " + grabber.width + " Height: " + grabber.height);
        IntPtr dest_tex = grabber.texture;
        Debug.Log("Dest tex: " + dest_tex);

        /*
        while (grabber.GetNextFrame (IntPtr.Zero) != 0)
            ;
        */
        texture = Texture2D.CreateExternalTexture(grabber.width, grabber.height, TextureFormat.BGRA32, false, false, grabber.texture);
        materials[0].mainTexture = texture;
        //texture.EncodeToPNG ();
        //texture.Apply (true);
	}

	// OnWillRenderObject is called once for each camera if the object is visible.
	void OnWillRenderObject()
    {
        float factor = 1000;
        Width = grabber.width;
        Height = grabber.height;
        transform.localScale = new Vector3(Width/factor, 1, Height/factor);

		Debug.Log("Got frame result: " + grabber.GetNextFrame(texture.GetNativeTexturePtr()));
		//Texture2D.CreateExternalTexture (grabber.width, grabber.height, TextureFormat.BGRA32, 0, true, nativeTex);
		//texture.UpdateExternalTexture(grabber.texture);
		//texture.Apply ();
    }

	class Grabber : IDisposable
	{
        private const string DllName = "unity-screen-capture-plugin";

        [DllImport(DllName)]
		private static extern IntPtr grabber_create(IntPtr texture);
		[DllImport(DllName)]
		private static extern void grabber_destroy(IntPtr grabber);
		[DllImport(DllName)]
		private static extern int grabber_get_next_frame(IntPtr grabber, IntPtr texture);
		[DllImport(DllName)]
		private static extern int grabber_get_width(IntPtr grabber);
		[DllImport(DllName)]
		private static extern int grabber_get_height(IntPtr grabber);
		[DllImport(DllName)]
		private static extern IntPtr grabber_get_dest_tex(IntPtr grabber);

		private IntPtr grabber;

		internal Grabber(IntPtr nativeTex)
		{
            Thread thread = Thread.CurrentThread;
			grabber = grabber_create(nativeTex);
			if (grabber.ToInt64() == 0)
			{
				throw new Exception("grabber_create failed");
			}
		}

        public void Dispose()
		{
			Thread thread = Thread.CurrentThread;
            grabber_destroy(grabber);
        }

        internal int GetNextFrame(IntPtr nativeTex)
		{
			return grabber_get_next_frame(grabber, nativeTex);
		}

		internal int width
		{
			get { return grabber_get_width(grabber); }
		}

		internal int height
		{
			get { return grabber_get_height(grabber); }
		}

		internal IntPtr texture
		{
			get { return grabber_get_dest_tex(grabber); }
		}
	}
}