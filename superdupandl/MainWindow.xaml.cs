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
            MyBaiduDownloader.testAdd();

            InitializeComponent();

            //Data binding listview and file list
            listView.ItemsSource = MyBaiduDownloader._fileDownloadList;

        }

       

        private void ListViewItem_Selected(object sender, RoutedEventArgs e)
      {

      }

        private void listView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }
    }
}
