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
        // for dupan remote file items,  
        public class RemoteFileListItem
        {
            public string filename { get; set; } // file path in baidu net disk
            public ulong size { get; set; }
            public DateTime datetime { get; set; }
            public string remotePath { get; set; } // file path in baidu net disk
            public bool isfolder { get; set; }


            public RemoteFileListItem(string file_name, ulong sizee, DateTime datetimee, string remotepath)
            {
                filename = file_name;
                size = sizee;
                datetime = datetimee;
                remotepath = remotePath;
            }

        };


        public class ToDownloadFileItem
        {

            public string filename { get; set; } // file path in baidu net disk
            public ulong size { get; set; }
            public DateTime datetime { get; set; }
            public string remotePath { get; set; } // file path in baidu net disk
            public bool isfolder { get; set; }


            public ToDownloadFileItem(string file_name, ulong sizee, DateTime datetimee, string remotepath)
            {
                filename = file_name;
                size = sizee;
                datetime = datetimee;
                remotepath = remotePath;
            }

        };





        public class RemotePathItem
        {
            public string foldername { get; set; }
            public RemotePathItem(string folder)
            {
                foldername = folder;
            }

        };

        // allow only one download list
        static public ObservableCollection<ToDownloadFileItem> _fileDownloadList = new ObservableCollection<ToDownloadFileItem>();

        static public ObservableCollection<RemoteFileListItem> _remoteFileList = new ObservableCollection<RemoteFileListItem>();
        

        // could be ie \home\pcgames2018\farcry5 ...  so 3 items in the list, the first item is always home or root or \
        static public ObservableCollection<RemotePathItem> _remotePathList = new ObservableCollection<RemotePathItem>();


        static public void testRemoteFileListAdd()
        {

            for (ulong i = 0; i < 10; i++)
            {
                string filename = "filename_" + i;
                string path = "pathdsfggd!!szdfdsfdasfs";
                ulong size = 100ul * i;
                RemoteFileListItem baiduFileItem = new RemoteFileListItem(filename, size, DateTime.Now, path);
                _remoteFileList.Add(baiduFileItem);
                Console.WriteLine(i);
            }
        }

        static public void testRemotePathAdd()
        {
            for (ulong i = 0; i < 5; i++)
            {
                RemotePathItem path = new RemotePathItem("remotePath_"  + i);

                _remotePathList.Add(path);
                Console.WriteLine("!!!!");
                Console.WriteLine(path.foldername);
            }

        }
    }
}


