
using UnityEngine;
using UnityEngine.Rendering;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public class Video : MonoBehaviour
{
    //the name of the DLL you want to load stuff from
    private const string pluginName = "unity-screen-capture-plugin";

    public delegate void DebugDelegate(string str);

    static void DebugCallBackFunction(string str)
    {
        Debug.Log(str);
    }

    [DllImport(pluginName)]
    public static extern void SetDebugFunction(IntPtr debugCallback);

    private CommandBuffer cmd;

    // Use this for initialization
    void Start()
    {
        DebugDelegate debug_callback_delegate = new DebugDelegate(DebugCallBackFunction);
        // Convert callback_delegate into a function pointer that can be
        // used in unmanaged code.
        IntPtr debug_intptr_delegate =
            Marshal.GetFunctionPointerForDelegate(debug_callback_delegate);
        // Call the API passing along the function pointer.
        SetDebugFunction(debug_intptr_delegate);
    }

    public void OnDestroy()
    {
    }

    // Update is called once per frame
    void Update()
    {
    }
}