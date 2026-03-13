# Local net
### Example local net:

### 1. AP
*In this mode, the ESP32 or a Phone acts as a central **Gateway/Router**, creating an independent wireless environment.*

```mermaid
graph TD
    GW(("<b>GATEWAY</b><br/>(ESP32 / Phone AP)"))
    
    subgraph Local_Subnet [<b>Private Node Space</b>]
        SRV[<b>PHONE SERVER</b><br/>(C++ Backend Node)]
        C1[Client Phone 1]
        C2[Client Phone 2]
        CN[Device N...]
    end

    SRV  <===> GW
    C1   ---  GW
    C2   ---  GW
    CN   -.-  GW

    style GW fill:#1a1a1a,stroke:#fff,stroke-width:2px,color:#fff
    style SRV fill:#003300,stroke:#00ff41,stroke-width:2px,color:#00ff41
    style Local_Subnet fill:none,stroke:#333,stroke-dasharray: 5 5
   ```
   ### Home net
  graph TD
    Router{{"<b>EXTERNAL ROUTER</b><br/>(Home/Office Wi-Fi)"}}

    subgraph Infrastructure_LAN [<b>Shared Network</b>]
        ESP[<b>ESP32</b><br/>(Network Node)]
        SRV[<b>PHONE SERVER</b><br/>(C++ Backend Node)]
        C1[Client Phone 1]
        CN[Client Phone N]
    end

    ESP  ---  Router
    SRV  <===> Router
    C1   ---  Router
    CN   -.-  Router

    style Router fill:#001f3f,stroke:#0074D9,stroke-width:2px,color:#fff
    style SRV fill:#003300,stroke:#00ff41,stroke-width:2px,color:#00ff41
    style ESP fill:#331a00,stroke:#ff851b,stroke-width:2px,color:#ff851b
    style Infrastructure_LAN fill:none,stroke:#333,stroke-dasharray: 5 5
