using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
 
namespace superdupandl
{

  


   /// <summary>
   /// Interaction logic for DupanFileListWindow.xaml
   /// </summary>
   public partial class DupanFileListWindow : Window
   {

     

      public DupanFileListWindow()
      {
            this.DataContext = this;


            MyBaiduDownloader.testRemoteFileListAdd();
            MyBaiduDownloader.testRemotePathAdd();


            InitializeComponent();

            //Data binding listview and file list
            listView_fileList.ItemsSource = MyBaiduDownloader._remoteFileList;

            
            listview_pathSelector.ItemsSource = MyBaiduDownloader._remotePathList;

         


        }

       

        private void ListViewItem_Selected(object sender, RoutedEventArgs e)
      {

      }

        private void listView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void buttonDownload_Click(object sender, RoutedEventArgs e)
        {

            if (checkBoxAll.IsChecked==true)
            {
                listView_fileList.SelectAll();
            }

            int count = listView_fileList.SelectedItems.Count;
            for (int i = 0; i < count; i++)
            {
                superdupandl.MyBaiduDownloader.RemoteFileListItem x = (superdupandl.MyBaiduDownloader.RemoteFileListItem ) listView_fileList.SelectedItems[i];
                Console.WriteLine(x.filename + " "+ x.size);
            }

            checkBoxAll.IsChecked = false;
            listView_fileList.UnselectAll();
        }

        private void checkBoxAll_Checked(object sender, RoutedEventArgs e)
        {
            listView_fileList.SelectAll();
        }


        private void checkBoxAll_unchecked(object sender, RoutedEventArgs e)
        {
            listView_fileList.UnselectAll();
        }
 
    }
}
