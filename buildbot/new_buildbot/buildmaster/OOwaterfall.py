class OOStatusResourceBuilder(StatusResourceBuilder):
    def body(self, request):
        b = self.builder
        slaves = b.getSlaves()
        connected_slaves = [s for s in slaves if s.isConnected()]

        buildbotURL = self.status.getBuildbotURL()
        projectName = self.status.getProjectName()
        data = "<a href=\"%s\">%s</a>\n" % (buildbotURL, projectName)
        data += make_row("Builder:", html.escape(b.getName()))
        b1 = b.getBuild(-1)
        if b1 is not None:
            data += make_row("Current/last build:", str(b1.getNumber()))
        data += "\n<br />BUILDSLAVES<br />\n"
        data += "<ol>\n"
        for slave in slaves:
            data += "<li><b>%s</b>: " % html.escape(slave.getName())
            if slave.isConnected():
                data += "CONNECTED\n"
                if slave.getAdmin():
                    data += make_row("Admin:", html.escape(slave.getAdmin()))
                if slave.getHost():
                    data += "<span class='label'>Host info:</span>\n"
                    data += html.PRE(slave.getHost())
            else:
                data += ("NOT CONNECTED\n")
            data += "</li>\n"
        data += "</ol>\n"

        if self.control is not None and connected_slaves:
            forceURL = urllib.quote(request.childLink("force"))
            data += (
                """
                <form action='%(forceURL)s' class='command forcebuild'>
                <p>To force a build, fill out the following fields and
                push the 'Force Build' button</p>
                <table border='0'>
                  <tr>
                    <td>
                      Your name:
                    </td>
                    <td>
                      <input type='text' name='username' />@openoffice.org (for email notification about build status)
                    </td>
                 </tr>
                 <tr>
                    <td>
                       Reason for build:
                    </td>
                    <td>
                       <input type='text' name='comments' />
                    </td>
                 </tr>
                 <tr>
                    <td>
                       CWS to build:
                    </td>
                    <td>
                        <input type='text' name='branch' />(e.g. configdbbe, kaib01, ww8perf02)
                    </td>
                 </tr>
                 <tr>
                    <td>
                        Config Switches:
                    </td>
                    <td>
                        <input type='text' size='50' name='config' />(if your CWS requires extra config switches)
                    </td>
                 </tr>
                 <tr>
                    <td>
                        Make Install-Set:
                    </td>
                    <td>
                         <input type='checkbox' name='installsetcheck' />(If you want to download install-sets)
                    </td>
                 </tr>
                 <tr>
                   <td colspan='2'>
                       <input type='submit' value='Force Build' />
                   </td>
                 </tr>
                </table>
                </form>
                """) % {"forceURL": forceURL}
        elif self.control is not None:
            data += """
            <p>All buildslaves appear to be offline, so it's not possible
            to force this build to execute at this time.</p>
            """

        if self.control is not None:
            pingURL = urllib.quote(request.childLink("ping"))
            data += """
            <form action="%s" class='command pingbuilder'>
            <p>To ping the buildslave(s), push the 'Ping' button</p>

            <input type="submit" value="Ping Builder" />
            </form>
            """ % pingURL

        return data
