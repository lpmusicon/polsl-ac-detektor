import styled from "styled-components";

export const Button = styled.button`
  outline: none;
  width: 100%;
  background: #f59f00;
  color: #002024;
  font-size: 18px;
  font-family: "Montserrat", arial, sans-serif;
  border: none;
  border-radius: 5px;
  padding: 14px;
  cursor: pointer;
  transition: background 0.3s ease-in-out, filter 0.3s ease-in-out;
  filter: drop-shadow(0 0 0 rgba(0, 0, 0, 0));

  &:hover {
    background: #ffb833;
    filter: drop-shadow(0 1px 4px rgba(255, 184, 51, 0.4));
  }

  &:focus {
    background: #e09200;
    filter: drop-shadow(0 1px 4px rgba(255, 184, 51, 0.4));
  }
`;
