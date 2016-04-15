/*
 * RUBiS
 * Copyright (C) 2002, 2003, 2004 French National Institute For Research In Computer
 * Science And Control (INRIA).
 * Contact: jmob@objectweb.org
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * Initial developer(s): Emmanuel Cecchet, Julie Marguerite
 * Contributor(s): Jeremy Philippe, Niraj Tolia
 */

package edu.rice.rubis.client;

import java.io.*;
import java.util.ArrayList;
import java.util.GregorianCalendar;
import java.util.List;

import edu.rice.rubis.client.RUBiSProperties;
import edu.rice.rubis.client.Stats;
import edu.rice.rubis.client.TransitionTable;
import edu.rice.rubis.client.URLGenerator;
import edu.rice.rubis.client.UserSession;

/**
 * RUBiS client emulator.
 * This class plays random user sessions emulating a Web browser.
 *
 * @author <a href="mailto:cecchet@rice.edu">Emmanuel Cecchet</a> and <a href="mailto:julie.marguerite@inrialpes.fr">Julie Marguerite</a>
 *
 * @version 1.0
 */
public class ClientEmulator
{
    private RUBiSProperties rubis = null; // access to rubis.properties file
    private URLGenerator urlGen = null;
    // URL generator corresponding to the version to be used (PHP, EJB or Servlets)
    private static float slowdownFactor = 0;
    private static boolean endOfSimulation = false;

    // Added by FD
    private List<Integer> nUsers = null;
    private List<Integer> uBegin = null;
    private int numberOfUsers = 0;
    private static long numberOfRequests = 0;

    /**
     * Creates a new <code>ClientEmulator</code> instance.
     * The program is stopped on any error reading the configuration files.
     */
    public ClientEmulator(String propertiesFileName)
    {
        // Initialization, check that all files are ok
        rubis = new RUBiSProperties(propertiesFileName);
        urlGen = rubis.checkPropertiesFileAndGetURLGenerator();
        if (urlGen == null)
            Runtime.getRuntime().exit(1);
        // Check that the transition table is ok and print it
        TransitionTable transition =
                new TransitionTable(
                        rubis.getNbOfColumns(),
                        rubis.getNbOfRows(),
                        null,
                        rubis.useTPCWThinkTime());
        if (!transition.ReadExcelTextFile(rubis.getTransitionTable()))
            Runtime.getRuntime().exit(1);

        // Init lists of number of users
        nUsers = new ArrayList<>();
        uBegin = new ArrayList<>();

        try(BufferedReader br = new BufferedReader(new FileReader("usersFile.txt"))) {
            String current;
            String parts[];
            while((current = br.readLine()) != null) {
                parts = current.split(" ");
                nUsers.add(Integer.parseInt(parts[0]));
                uBegin.add(Integer.parseInt(parts[1]));
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        for (int n : nUsers) {
            numberOfUsers += n;
        }
    }

    /**
     * Updates the slowdown factor.
     *
     * @param newValue new slowdown value
     */
    private synchronized void setSlowDownFactor(float newValue)
    {
        slowdownFactor = newValue;
    }

    /**
     * Get the slowdown factor corresponding to current ramp (up, session or down).
     *
     * @return slowdown factor of current ramp
     */
    public static synchronized float getSlowDownFactor()
    {
        return slowdownFactor;
    }

    public static synchronized void addRequests(long requests) {
        numberOfRequests += requests;
    }

    /**
     * Set the end of the current simulation
     */
    private synchronized void setEndOfSimulation()
    {
        endOfSimulation = true;
    }

    /**
     * True if end of simulation has been reached.
     * @return true if end of simulation
     */
    public static synchronized boolean isEndOfSimulation()
    {
        return endOfSimulation;
    }

    public int getNumberOfUsers() {
        return numberOfUsers;
    }

    public List<Integer> getnUsers() {
        return nUsers;
    }

    public List<Integer> getuBegin() {
        return uBegin;
    }

    /**
     * Main program take an optional output file argument only
     * if it is run on as a remote client.
     *
     * @param args optional output file if run as remote client
     */
    public static void main(String[] args)
    {
        GregorianCalendar startDate;
        GregorianCalendar endDate;
        GregorianCalendar upRampDate;
        GregorianCalendar runSessionDate;
        GregorianCalendar downRampDate;
        GregorianCalendar endDownRampDate;
        Process webServerMonitor = null;
        Process cjdbcServerMonitor = null;
        Process[]         dbServerMonitor = null;
        Process[]         ejbServerMonitor = null;
        Process[]         servletsServerMonitor = null;
        Process clientMonitor;
        Process[] remoteClientMonitor = null;
        Process[] remoteClient = null;
        String reportDir = "";
        String tmpDir = "/tmp/";

        long startSession = 0;
        long endSession = 0;

        boolean           isMainClient = (args.length <= 2); // Check if we are the main client
        String propertiesFileName;

        if (isMainClient)
        {
            // Start by creating a report directory and redirecting output to an index.html file
            System.out.println(
                    "RUBiS client emulator - (C) Rice University/INRIA 2001\n");

            if (args.length == 0)
                propertiesFileName = "rubis";
            else
                propertiesFileName = args[0];
        }
        else
        {
            System.out.println(
                    "RUBiS remote client emulator - (C) Rice University/INRIA 2001\n");
            startDate = new GregorianCalendar();
            propertiesFileName = args[2];
        }

        ClientEmulator client = new ClientEmulator(propertiesFileName);
        // Get also rubis.properties info

        Stats stats = new Stats(client.rubis.getNbOfRows());
        UserSession[] sessions = new UserSession[client.getNumberOfUsers()];

        TransitionTable transition = new TransitionTable(client.rubis.getNbOfColumns(), client.rubis
                .getNbOfRows(), stats, client.rubis.useTPCWThinkTime());
        if (!transition.ReadExcelTextFile(client.rubis.getTransitionTable()))
            Runtime.getRuntime().exit(1);

        // #############################
        // ### TEST TRACE BEGIN HERE ###
        // #############################
        // Run user sessions
        System.out.println(
                "ClientEmulator: Starting "
                        + client.getNumberOfUsers()
                        + " session threads");

        startSession = System.currentTimeMillis();

        int sessionN = 0;
        for (int i = 0; i < client.getnUsers().size(); i++) {
            for (int j = 0; j < client.getnUsers().get(i); j++) {
                sessions[sessionN] = new UserSession("UserSession" + i, client.urlGen, client.rubis, stats,
                        client.getuBegin().get(i),transition);
                sessions[sessionN].start();
                sessionN++;
            }
        }

        // Start up-ramp
        client.setSlowDownFactor(client.rubis.getUpRampSlowdown());

        try
        {
            Thread.currentThread().sleep(client.rubis.getUpRampTime());
        }
        catch (java.lang.InterruptedException ie)
        {
            System.err.println("ClientEmulator has been interrupted.");
        }

        client.setSlowDownFactor(1);

        try
        {
            Thread.currentThread().sleep(client.rubis.getSessionTime());
        }
        catch (java.lang.InterruptedException ie)
        {
            System.err.println("ClientEmulator has been interrupted.");
        }

        client.setSlowDownFactor(client.rubis.getDownRampSlowdown());

        try
        {
            Thread.currentThread().sleep(client.rubis.getDownRampTime());
        }
        catch (java.lang.InterruptedException ie)
        {
            System.err.println("ClientEmulator has been interrupted.");
        }

        // Wait for completion
        client.setEndOfSimulation();
        System.out.println("ClientEmulator: Shutting down threads ...");
        for (int i = 0; i < client.rubis.getNbOfClients(); i++)
        {
            try
            {
                sessions[i].join(2000);
            }
            catch (java.lang.InterruptedException ie)
            {
                System.err.println(
                        "ClientEmulator: Thread " + i + " has been interrupted.");
            }
        }
        endSession = System.currentTimeMillis();

        System.out.println("Session time: " + (endSession - startSession));

        System.out.println("Request rate: " + (numberOfRequests / (endSession - startSession) / 1000));

        System.out.println("Done\n");

        Runtime.getRuntime().exit(0);
    }

}
