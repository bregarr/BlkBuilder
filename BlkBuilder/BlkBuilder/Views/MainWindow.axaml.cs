using System;
using Avalonia.Controls;
using Avalonia.Interactivity;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using Avalonia.Platform.Storage;

namespace BlkBuilder.Views;

public enum AwokFileType
{
	blk,
	spa
}
public partial class MainWindow : Window
{
	[DllImport("libBlkBuilderDll.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern void BB_Decomp(string fileName);
	[DllImport("libBlkBuilderDll.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern void BB_STP(string fileName);
	[DllImport("libBlkBuilderDll.dll", CallingConvention = CallingConvention.Cdecl)]
	private static extern void BB_PTS(string newFilePath, float fps, long flagsPass, short modePass, [In] string[] fileVec, int fileCount);

	private string _FILEPATH;
	private AwokFileType _FILETYPE;
	private List<string> _INPUTFILES;
	
	public MainWindow()
	{
		InitializeComponent();
	}

	private async void UploadFile(object sender, RoutedEventArgs args)
	{
		var topLevel = TopLevel.GetTopLevel(this);
		var files = await topLevel.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
		{
			Title = "Select BLK File",
			AllowMultiple = false,
			FileTypeFilter = new[] { AwokTypes }
		});

		if (files.Count >= 1){
			var filePath = files[0].Path.LocalPath;
			var fileExt = filePath.Substring(filePath.Length - 3);
			_FILEPATH = filePath;
			FileSelectName.Text = filePath;
			
			if(fileExt.ToLower() == "blk"){
				_FILETYPE = AwokFileType.blk;
				UpdateDescText();
			}
			else if(fileExt.ToLower() == "spa"){
				_FILETYPE = AwokFileType.spa;
				UpdateDescText();
			}
		}
	}

	private async void ChooseLocation(object sender, RoutedEventArgs args){
		var topLevel = TopLevel.GetTopLevel(this);
		var file = await topLevel.StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
		{
			Title = "Select SPA Output Location",
			DefaultExtension = ".SPA",
			FileTypeChoices = new [] { AwokTypes }
		});

		_FILEPATH = file.Path.LocalPath;
		LocationSelectName.Text = _FILEPATH;
	}

	private async void UploadFiles(object sender, RoutedEventArgs args){
		var topLevel = TopLevel.GetTopLevel(this);
		var files = await topLevel.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions{
			Title = "Select PNG Input Files",
			FileTypeFilter = new[] { FilePickerFileTypes.ImagePng },
			AllowMultiple = true
		});

		if (files.Count >= 1){
			_INPUTFILES = new List<string>();
			FilesSelectName.Text = "";
			for (int i = 0; i < files.Count; i++){
				_INPUTFILES.Add(files[i].Path.LocalPath);
				if (i > 0) FilesSelectName.Text += ", ";
				FilesSelectName.Text += files[i].Path.LocalPath;
			}
		}
	}
	public void DecompButtonHandler(object sender, RoutedEventArgs args){
		if (_FILEPATH == null || _FILETYPE == null) return;
		try{
			if (_FILETYPE == AwokFileType.blk){
				BB_Decomp(_FILEPATH);
			} else if (_FILETYPE == AwokFileType.spa){
				BB_STP(_FILEPATH);
			}
			else FileSelectName.Text = "Please select a valid BLK file!";
		}
		catch(Exception e){
			Console.WriteLine($"Error with file!: {e}");
		}
	}

	public void CompButtonHandler(object sender, RoutedEventArgs args){
		if (_FILEPATH == null) return;
		try{
			CompilePNGToSPA();
		}
		catch(Exception e){
			Console.WriteLine($"Error with file!: {e}");
		}
	}
	
	private void CompilePNGToSPA(){
		float fps;
		long flagsPass;
		short modePass;
		try{ fps = float.Parse(FPSBox.Text); }
		catch{ CompErrorText.Text = "Please enter a valid float for FPS!"; return; }
		try{ flagsPass = long.Parse(FlagsBox.Text); }
		catch{ CompErrorText.Text = "Please enter a valid integer for Flags!"; return; }
		try{ modePass = short.Parse(ModeBox.Text); }
		catch{ CompErrorText.Text = "Please enter a valid integer for Mode!"; return; }
		Console.WriteLine($"Building File {_FILEPATH}");
		BB_PTS(_FILEPATH, fps, flagsPass, modePass, _INPUTFILES.ToArray(), _INPUTFILES.Count);
	}
	
	// Utils
	private void UpdateDescText()
	{
		var newType = _FILETYPE;
		if (newType == AwokFileType.blk) DecompButton.Content = "Decompress BLK File!";
		else if (newType == AwokFileType.spa) DecompButton.Content = "Convert SPA File!";
	}
	
	public static FilePickerFileType AwokTypes { get; } = new("Awok Types")
	{
		Patterns = new[] { "*.blk", "*.spa" }
	};
}