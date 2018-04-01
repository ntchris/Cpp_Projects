using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace superdupandl
{

    
     

    static class MyBaiduDownloader
    {

        public class ToDownloadFileItem
        {

            public string filename { get; set; } // file path in baidu net disk
            public ulong size { get; set; }
            public DateTime datetime { get; set; }
            public string remotePath { get; set; } // file path in baidu net disk
             

            public ToDownloadFileItem(string file_name, ulong sizee, DateTime datetimee, string remotepath)
            {
                filename = file_name;
                size = sizee;
                datetime = datetimee;
                remotepath = remotePath;
            }

        };

        

        // allow only one download list
        static public ObservableCollection<ToDownloadFileItem> _fileDownloadList = new ObservableCollection<ToDownloadFileItem>();
         
        static public void testAdd()
        {

            for (ulong i = 0; i < 10; i++)
            {
                string filename = "filename_" + i;
                string path = "pathdsfggd!!szdfdsfdasfs";
                ulong size = 100ul * i;
                ToDownloadFileItem baiduFileItem = new ToDownloadFileItem(filename, size, DateTime.Now, path);
                _fileDownloadList.Add(baiduFileItem);
                Console.WriteLine(i);
            }
        }
    };
}
