

import com.sun.istack.internal.NotNull;

import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.util.StringTokenizer;

public class Client
{
    private static int ServerPort;
    private static String clientName = null;
    private  static String ipAddr;
    public static void main(String args[]) throws UnknownHostException, IOException
    {
        if(args.length < 3 || args.length > 3){
            System.out.println("bash ./client.sh <username> <ip> <port>");
            System.exit(0);
        }
        clientName = args[0];
        ServerPort = Integer.parseInt(args[2]);
        ipAddr = args[1];

        Scanner scn = new Scanner(System.in);


        InetAddress ip = InetAddress.getByName(ipAddr);


        Socket s = new Socket(ip, ServerPort);

        DataInputStream dis = new DataInputStream(s.getInputStream());
        DataOutputStream dos = new DataOutputStream(s.getOutputStream());

        try{
            dos.writeUTF(clientName);
        } catch (IOException e){
            e.printStackTrace();
        }


        Thread sendMessage = new Thread(new Runnable()
        {
            @Override
            public void run() {
                while (true) {

                    String msg = scn.nextLine();

                    try {

                        dos.writeUTF(msg);
                        if(msg.contains("send")){
                            StringTokenizer st = new StringTokenizer(msg);
                            st.nextToken();
                            String fileName = st.nextToken();
                            ServerSocket socket = new ServerSocket(9000);
                            Socket client = socket.accept();
                            byte[] buffer = new byte[8192];
                            String connection_type = st.nextToken();
                            if(connection_type.equals("tcp")) {
                                BufferedInputStream in =
                                        new BufferedInputStream(
                                                new FileInputStream("./" + clientName + "/" + fileName));

                                BufferedOutputStream out =
                                        new BufferedOutputStream(client.getOutputStream());


                                System.out.println("Sending " + fileName);
                                int len = 0;
                                while ((len = in.read(buffer)) > 0) {
                                    out.write(buffer, 0, len);
                                }
                                in.close();
                                out.flush();
                                out.close();
                                client.close();
                                socket.close();
                                System.out.println("Sent file");
                            }
                            else if (connection_type.equals("udp")) {
                                BufferedInputStream in =
                                        new BufferedInputStream(
                                                new FileInputStream("./" + clientName + "/" + fileName));

                                BufferedOutputStream out =
                                        new BufferedOutputStream(client.getOutputStream());


                                System.out.println("Sending " + fileName);
                                int len = 0;
                                while ((len = in.read(buffer)) > 0) {
                                    out.write(buffer, 0, len);
                                }
                                in.close();
                                out.flush();
                                out.close();
                                client.close();
                                socket.close();
                                System.out.println("Sent file");
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    if(msg == "logout") break;
                }
            }
        });

        Thread readMessage = new Thread(new Runnable()
        {
            @Override
            public void run() {

                while (true) {
                    try {

                        String msg = dis.readUTF();
                        if(msg.contains("FT:TCP")){
                            StringTokenizer st = new StringTokenizer(msg);
                            st.nextToken();
                            String fileName = st.nextToken();
                            Socket socket = new Socket("localhost", 9001);
                            BufferedInputStream in =
                                    new BufferedInputStream(socket.getInputStream());

                            BufferedOutputStream out =
                                    new BufferedOutputStream(new FileOutputStream("./" + clientName + "/" + fileName));
                            int len = 0;
                            byte [] buffer = new byte[8192];
                            while ((len = in.read(buffer)) > 0) {
                                out.write(buffer, 0, len);
                            }
                            in.close();
                            out.flush();
                            out.close();
                            socket.close();
                        }
                        else if(msg.contains("FT:UDP")){
                            StringTokenizer st = new StringTokenizer(msg);
                            st.nextToken();
                            String fileName = st.nextToken();
                            Socket socket = new Socket("localhost", 9001);
                            BufferedInputStream in =
                                    new BufferedInputStream(socket.getInputStream());

                            BufferedOutputStream out =
                                    new BufferedOutputStream(new FileOutputStream("./" + clientName + "/" + fileName));
                            int len = 0;
                            byte [] buffer = new byte[8192];
                            while ((len = in.read(buffer)) > 0) {
                                out.write(buffer, 0, len);
                            }
                            in.close();
                            out.flush();
                            out.close();
                            socket.close();
                        }
                        else if(msg.contains("Client limit reached")) {
                            System.out.println("Client limit has been reached");
                            System.exit(0);

                        }
                        else if(msg.contains("User already exists")) {
                            System.out.println("User already exists");
                            System.exit(0);

                        }
                        else System.out.println(msg);
                    } catch (IOException e) {

                        e.printStackTrace();
                    }
                }
            }
        });

        sendMessage.start();
        readMessage.start();

    }
}