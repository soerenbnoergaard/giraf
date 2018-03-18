# Giraf - Real-time CSV file plotting utility

This program aims to provide a simple command line interface for opening a graph where the CSV data is plotted while it is being written by another process. 

Being able to visualize CSV data in real-time is very useful, for example, when doing automated measurements where the data is appended to a CSV file continuously. Being able to see the data makes any erroneous measurements easy to spot.

The program is written in Qt Creator, should anyone wish to modify it.

## Examples

### Read from CSV file

To read the file `test.csv` looking like this:

    Frequency [MHz],Power [dBm],EVM [dB]
    2402,20.0,-30.0
    2412,19.5,-28.1
    2422,19.6,-29.0
    2432,21.2,-30.3
    2442,20.2,-30.2
    2452,20.0,-25.1
    2462,21.0,-24.0
    2472,20.1,-22.5

where the first row contains labels, the first column contains x-axis data, and the two last rows contain measurement data, the following command is executed (note, the indexing is done from 1 to comply with most spreadsheet software):

    giraf -l1 -x1 -y2 -y3 test.csv

or simply (exploiting the defaults):

    giraf -x1 -y2 -y3 test.csv

The resulting graph is then

![Screenshot 001](img/screenshot_001.png)

and when new lines are appended to `test.csv`, the graph will update as well.


### Data from the `stdin` pipe

Data can also be read from `stdin`, making it possible to pipe CSV data to the program. The example from above can therefore also be written as

    cat test.csv | giraf -x1 -y2 -y3

This makes it possible to plot data from many different sources.

_Note that many processes have output buffering enabled._ To get real-time data, the output of the data stream should be line buffered. On linux, the "unbuffer" utility may be helpful. For example,

    unbuffer ./read_from_data_source | giraf -l0 -x1 -y2

It would be great to have `giraf` handle this, but I am not sure it is possible as the buffering is done in the source process. Any suggestions are more than welcome!


## Commands and Keyboard shortcuts

The specific command line options are seen by executing

    giraf -h

The following keyboard shortcuts are available:

- `<Space>` Reload file.
- `<Ctrl-s>` Save image.
- `<Ctrl-c>` Copy to clipboard.


