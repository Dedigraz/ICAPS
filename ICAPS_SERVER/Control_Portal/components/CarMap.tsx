import React, { useState } from 'react';
import "leaflet/dist/leaflet.css";
import "../styles/Home.css";
import { MapContainer, TileLayer, Marker, Popup, LayersControl, Polyline, useMapEvents } from "react-leaflet";
import type { IconOptions, LatLngTuple } from "leaflet";
import L from "leaflet";
import markerIcon from "leaflet/dist/images/marker-icon.png";
import type {Anomaly, CarMapProps} from "@/utils/types";

const DefaultIcon = L.icon({
  iconUrl: markerIcon.src??markerIcon,
  iconSize: [25, 41],
  iconAnchor: [12, 41],
  popupAnchor: [1, -34],
});

function CarMap({ carLocation, anomalies, route,routeHandler }: CarMapProps) {
  const [selectedPoints, setSelectedPoints] = useState<LatLngTuple[]>([]);
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

  function RouteComponent(){
  const map = useMapEvents({
    dblclick: (e) => {
      const newPoint: LatLngTuple = [e.latlng.lat, e.latlng.lng];
      setSelectedPoints((prevPoints) => {
        if (prevPoints.length < 2) {
          return [...prevPoints, newPoint];
        }
        return prevPoints;
      });
    },
  } );
  return (
    <LayersControl.Overlay checked name="Route">
    {
      selectedPoints.map((point, i) => (
        <Marker position={point} icon={DefaultIcon} key={i} zIndexOffset={1000}>
          <Popup>Point {i + 1}</Popup>
        </Marker>
      ))
    }
    <Polyline positions={route} color="blue" />
  </LayersControl.Overlay>
  )
  } 
  const handleRoute = () => {
    if (selectedPoints.length === 2) {
      routeHandler(selectedPoints[0], selectedPoints[1]);
    }
  };

  return (
    <MapContainer
      className="map"
      center={carLocation}
      zoom={18}
      scrollWheelZoom={false}
      zoomAnimation={true}
      doubleClickZoom={false}
    
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
          <RouteComponent />
      </LayersControl>
    </MapContainer>
  );
}

export default CarMap;
