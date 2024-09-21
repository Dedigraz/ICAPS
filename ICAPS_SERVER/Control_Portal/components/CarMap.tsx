import React from 'react';
import "leaflet/dist/leaflet.css";
import "../styles/Home.css";
import { MapContainer, TileLayer, Marker, Popup, LayersControl, Polyline } from "react-leaflet";
import type { IconOptions, LatLngTuple } from "leaflet";
import L from "leaflet";
import type {Anomaly, CarMapProps} from "@/utils/types";

function CarMap({ carLocation, anomalies, route }: CarMapProps) {
  console.log(anomalies);
  console.log(route);
  console.log(carLocation);

  const carIconOptions: IconOptions = {
    iconUrl: "car-marker.png",
    iconSize: [64.5, 38.7],
    iconAnchor: [22, 94],
    popupAnchor: [-3, -76],
  };
  const mineIconOptions: IconOptions = {
    iconUrl: "mine-marker.png",
    iconSize: [64.5, 38.7],
    iconAnchor: [22, 94],
    popupAnchor: [-3, -76],
  };
  const carIcon = L.icon(carIconOptions);
  const mineIcon = L.icon(mineIconOptions);

  return (
    <MapContainer
      className="map"
      center={carLocation}
      zoom={18}
      scrollWheelZoom={false}
      zoomAnimation={true}
    >
      <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />
      <LayersControl>
        <LayersControl.Overlay checked name="Anomalies">
          {anomalies.map((anomaly, i) => (
            <Marker position={[anomaly.lat - 0.0004, anomaly.lng]} key={i} icon={mineIcon} zIndexOffset={0}>
              <Popup>Anomaly</Popup>
            </Marker>
          ))}
        </LayersControl.Overlay>
        <LayersControl.Overlay checked name="Car Layer">
          <Marker position={carLocation} icon={carIcon} zIndexOffset={1000}>
            <Popup>Car Marker</Popup>
          </Marker>
        </LayersControl.Overlay>
        <LayersControl.Overlay checked name="Route">
          <Polyline positions={route} color="blue" />
        </LayersControl.Overlay>
      </LayersControl>
    </MapContainer>
  );
}

export default CarMap;
