package main

import (
	"fmt"
	"log"
	"os"
	"os/user"
	"time"

	"github.com/tarm/serial"
)

func main() {

	csvfile, err := os.OpenFile(getDataFileName(), os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatal(err)
	}

	c := &serial.Config{Name: "COM5", Baud: 9600}
	port, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	lineChannel := make(chan string)

	go readFromDevice(port, lineChannel)

	for line := range lineChannel {
		fmt.Println(line)
		if len(line) >= 5 && line[0:5] == "DATA," {
			csvfile.WriteString(line[5:] + "\r\n")
		}
		if len(line) >= 6 && line[0:6] == "LABEL," {
			csvfile.WriteString(line[6:] + "\r\n")
		}
	}
}

func getDataFileName() string {
	usr, err := user.Current()
	if err != nil {
		log.Fatal(err)
	}
	desktop := usr.HomeDir + "/Desktop"
	os.MkdirAll(desktop, 0644)
	timeFormat := "Jan 2 15.04.05 MST 2006"
	return desktop + "/ponddata-" + time.Now().Format(timeFormat) + ".csv"

}

func readFromDevice(port *serial.Port, outgoingLines chan string) {
	straightBuffer := make([]byte, 4*256)
	begin := 0

	for {

		n, err := port.Read(straightBuffer[begin:])
		if err != nil {
			panic("here")
			log.Fatal(err)
		}

		begin = sendLinesFromBuffer(straightBuffer, begin+n, outgoingLines)
	}
}

func sendLinesFromBuffer(buffer []byte, n int, channel chan string) int {

	tempStr := ""
	for i := 0; i < n; i++ {
		if buffer[i] == '\n' {
			channel <- tempStr
			tempStr = ""
			continue
		}
		if buffer[i] == '\r' {
			continue
		}
		tempStr += string(buffer[i])
	}

	for i := 0; i < len(tempStr); i++ {
		buffer[i] = tempStr[i]
	}

	return len(tempStr)
}
