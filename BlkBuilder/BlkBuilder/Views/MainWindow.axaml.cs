using System;
using Avalonia.Controls;
using Avalonia.Interactivity;
using System.Collections.Generic;
using System.IO;
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

	private string _FILEPATH;
	private AwokFileType _FILETYPE;
	
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
	public void DecompButtonHandler(object sender, RoutedEventArgs args)
	{
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