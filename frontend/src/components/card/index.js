import styled from "styled-components";

const CardStyled = styled.div`
  border-radius: 5px;
  border: 1px solid #44d194;
  background: linear-gradient(
    97.49deg,
    #083b4b 6.05%,
    rgba(7, 32, 44, 0) 94.12%
  );
  padding: 10px 10px 14px;
  display: grid;
  grid-template-areas:
    "icon time"
    "title title";
  justify-content: flex-start;
  align-items: center;
`;

const CardIcon = styled.img`
  display: block;
  grid-area: icon;
  width: 46px;
  height: 46px;
  object-fit: contain;
  margin-right: 12px;
`;
const CardDate = styled.p`
  font-size: 24px;
  color: #44d194;
  grid-area: time;
  margin: 0;
`;
const CardTitle = styled.p`
  font-size: 24px;
  color: #becbd8;
  grid-area: title;
  margin: 0;
`;

export const Card = ({ icon, date, title }) => {
  return (
    <CardStyled>
      <CardIcon src={icon} />
      <CardDate>{date}</CardDate>
      <CardTitle>{title}</CardTitle>
    </CardStyled>
  );
};
